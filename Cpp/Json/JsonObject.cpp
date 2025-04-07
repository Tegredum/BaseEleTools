#include "JsonObject.h"
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <direct.h> // for _mkdir on Windows
#include <sys/stat.h>

namespace Teg {
namespace Json {

bool JsonObject::createDirectory(const std::string& path) {
	size_t pos = 0;
	do {
		pos = path.find_first_of("\\/", pos + 1);
		std::string subdir = path.substr(0, pos);
		if (subdir.empty()) continue;
		
		struct stat info;
		if (stat(subdir.c_str(), &info) != 0) {
			// Directory doesn't exist, create it
			if (_mkdir(subdir.c_str()) != 0 && errno != EEXIST) {
				return false;
			}
		} else if (!(info.st_mode & S_IFDIR)) {
			// Path exists but is not a directory
			return false;
		}
	} while (pos != std::string::npos);
	return true;
}

void JsonObject::serializeToFile(const std::string& filePath) const {
	// Extract directory path
	size_t lastSlash = filePath.find_last_of("\\/");
	if (lastSlash != std::string::npos) {
		std::string dirPath = filePath.substr(0, lastSlash);
		if (!createDirectory(dirPath)) {
			throw std::runtime_error("Failed to create directory: " + dirPath);
		}
	}
	
	// Open file and write JSON
	std::ofstream outFile(filePath.c_str());
	if (!outFile.is_open()) {
		throw std::runtime_error("Failed to open file: " + filePath);
	}
	
	outFile << this->serializeFormatted();
	outFile.close();
}

JsonObject::JsonObject() : m_type(JSON_NULL) {
	memset(&m_number, 0, sizeof(m_number));
}

JsonObject::JsonObject(const std::string& str) : m_type(JSON_STRING) {
	m_string = new std::string(str);
}

JsonObject::JsonObject(double num) : m_type(JSON_NUMBER) {
	m_number = num;
}

JsonObject::JsonObject(bool b) : m_type(JSON_BOOL) {
	m_bool = b;
}

JsonObject::JsonObject(const JsonObject& other) : m_type(other.m_type) {
	this->m_array = nullptr;
	this->m_bool = false;
	this->m_number = 0.0;
	this->m_object = nullptr;
	this->m_string = nullptr;
	switch (m_type) {
		case JSON_NULL: break;
		case JSON_BOOL: m_bool = other.m_bool; break;
		case JSON_NUMBER: m_number = other.m_number; break;
		case JSON_STRING: m_string = new std::string(*other.m_string); break;
		case JSON_ARRAY: {
			m_array = new std::vector<JsonObject*>(other.m_array->size());
			for (int idx = 0; idx < m_array->size(); ++idx) {
				(*m_array)[idx] = new JsonObject(*((*(other.m_array))[idx]));
			}
			break;
		}
		case JSON_OBJECT: {
			m_object = new std::map<std::string, JsonObject*>;
			for (std::map<std::string, JsonObject*>::const_iterator iterator = other.m_object->begin(); iterator != other.m_object->end(); ++iterator) {
				std::string key = iterator->first;
				auto value = iterator->second;
				(*m_object)[key] = new JsonObject(*value);
			}
			break;
		}
	}
}

JsonObject::~JsonObject() {
	switch (m_type) {
		case JSON_STRING: delete m_string; break;
		case JSON_ARRAY: {
			for (int idx = 0; idx < m_array->size(); ++idx) {
				delete (*m_array)[idx];
			}
			delete m_array;
			break;
		}
		case JSON_OBJECT: {
			for (std::map<std::string, JsonObject*>::const_iterator iterator = m_object->begin(); iterator != m_object->end(); ++iterator) {
				delete iterator->second;
			}
			delete m_object;
			break;
		}
		default: break;
	}
}

JsonObject::Type JsonObject::type() const {
	return m_type;
}

bool JsonObject::toBool() const {
	if (m_type != JSON_BOOL) throw std::runtime_error("Not a boolean value");
	return m_bool;
}

double JsonObject::toNumber() const {
	if (m_type != JSON_NUMBER) throw std::runtime_error("Not a number value");
	return m_number;
}

std::string JsonObject::toString() const {
	if (m_type != JSON_STRING) throw std::runtime_error("Not a string value");
	return *m_string;
}

std::vector<JsonObject> JsonObject::toArray() const {
	if (m_type != JSON_ARRAY) throw std::runtime_error("Not an array");
	std::vector<JsonObject> result(m_array->size());
	for (int idx = 0; idx < m_array->size(); ++idx) {
		result[idx] = JsonObject((*m_array)[idx]);
	}
	return result;
}

std::map<std::string, JsonObject> JsonObject::toObject() const {
	if (m_type != JSON_OBJECT) throw std::runtime_error("Not an object");
	std::map<std::string, JsonObject> result;
	for (std::map<std::string, JsonObject*>::const_iterator iterator = m_object->begin(); iterator != m_object->end(); ++iterator) {
		result[iterator->first] = JsonObject(*(iterator->second));
	}
	return result;
}

void JsonObject::add(const std::string& key, const JsonObject& value) {
	if (m_type != JSON_OBJECT) {
		if (m_type != JSON_NULL) {
			throw std::runtime_error("Cannot add key-value pair to non-object");
		}
		m_type = JSON_OBJECT;
		m_object = new std::map<std::string, JsonObject*>();
	}
	(*m_object)[key] = new JsonObject(value);
}

void JsonObject::add(const JsonObject& value) {
	if (m_type != JSON_ARRAY) {
		if (m_type != JSON_NULL) {
			throw std::runtime_error("Cannot add element to non-array");
		}
		m_type = JSON_ARRAY;
		m_array = new std::vector<JsonObject*>();
	}
	m_array->push_back(new JsonObject(value));
}

void Teg::Json::JsonObject::serializeToCSV(const std::string& filePath) const {
	if (m_type != JSON_ARRAY) {
		throw std::runtime_error("serializeToCSV() can only be called on JSON array");
	}
	
	// Check if array is empty
	if (m_array->empty()) {
		throw std::runtime_error("Cannot serialize empty array to CSV");
	}
	
	// Determine if we have a simple array or array of arrays
	bool isNestedArray = ((*m_array)[0]->m_type == JSON_ARRAY);
	
	// Validate all elements have same type
	for (int idx = 1; idx < m_array->size(); ++idx) {
		if ((*m_array)[idx]->m_type != (*m_array)[0]->m_type) {
			throw std::runtime_error("CSV serialization requires all array elements to be of same type");
		}
		if (isNestedArray && (*m_array)[idx]->m_array->size() != (*m_array)[0]->m_array->size()) {
			throw std::runtime_error("All nested arrays must have same length for CSV serialization");
		}
	}
	
	// Extract directory path and create if needed
	size_t lastSlash = filePath.find_last_of("\\/");
	if (lastSlash != std::string::npos) {
		std::string dirPath = filePath.substr(0, lastSlash);
		if (!createDirectory(dirPath)) {
			throw std::runtime_error("Failed to create directory: " + dirPath);
		}
	}
	
	// Open file
	std::ofstream outFile(filePath.c_str());
	if (!outFile.is_open()) {
		throw std::runtime_error("Failed to open file: " + filePath);
	}
	
	if (isNestedArray) {
		// Handle array of arrays (each sub-array becomes a row)
		for (int idx = 0; idx < m_array->size(); ++idx) {
			const JsonObject* row = (*m_array)[idx];
			for (int col = 0; col < row->m_array->size(); ++col) {
				if (col > 0) outFile << ",";
				
				const JsonObject* item = (*(row->m_array))[col];
				switch (item->m_type) {
					case JSON_STRING: {
						outFile << "\"";
						std::string str = *item->m_string;
						// Escape quotes in strings
						size_t quotePos = str.find('"');
						while (quotePos != std::string::npos) {
							str.replace(quotePos, 1, "\"\"");
							quotePos = str.find('"', quotePos + 2);
						}
						outFile << str << "\"";
						break;
					}
					case JSON_NUMBER:
						outFile << item->m_number;
						break;
					case JSON_BOOL:
						outFile << (item->m_bool ? "true" : "false");
						break;
					default:
						throw std::runtime_error("Unsupported type in nested array for CSV serialization");
				}
			}
			outFile << "\n";
		}
	} else {
		// Handle simple array (each element becomes a row with one column)
		for (int idx = 0; idx < m_array->size(); ++idx) {
			const JsonObject* item = (*m_array)[idx];
			switch (item->m_type) {
				case JSON_STRING: {
					outFile << "\"";
					std::string str = *item->m_string;
					// Escape quotes in strings
					size_t quotePos = str.find('"');
					while (quotePos != std::string::npos) {
						str.replace(quotePos, 1, "\"\"");
						quotePos = str.find('"', quotePos + 2);
					}
					outFile << str << "\"";
					break;
				}
				case JSON_NUMBER:
					outFile << item->m_number;
					break;
				case JSON_BOOL:
					outFile << (item->m_bool ? "true" : "false");
					break;
				default:
					throw std::runtime_error("Unsupported type in array for CSV serialization");
			}
			outFile << "\n";
		}
	}
	
	outFile.close();
}

} // namespace Json
} // namespace Teg

std::string Teg::Json::JsonObject::serializeFormatted(int indentLevel) const {
	std::ostringstream oss;
	std::string indent(indentLevel, '\t');
	std::string nextIndent(indentLevel + 1, '\t');
	
	switch (m_type) {
		case JSON_NULL: oss << "null"; break;
		case JSON_BOOL: oss << (m_bool ? "true" : "false"); break;
		case JSON_NUMBER: oss << m_number; break;
		case JSON_STRING: {
			oss << "\"";
			for (std::string::const_iterator it = m_string->begin(); it != m_string->end(); ++it) {
				switch (*it) {
					case '\"': oss << "\\\""; break;
					case '\\': oss << "\\\\"; break;
					case '\b': oss << "\\b"; break;
					case '\f': oss << "\\f"; break;
					case '\n': oss << "\\n"; break;
					case '\r': oss << "\\r"; break;
					case '\t': oss << "\\t"; break;
					default: oss << *it;
				}
			}
			oss << "\"";
			break;
		}
		case JSON_ARRAY: {
			if (m_array->empty()) {
				oss << "[]";
			} else {
				oss << "[\n" << nextIndent;
				bool first = true;
				for (int idx = 0; idx < m_array->size(); ++idx) {
					if (!first) oss << ",\n" << nextIndent;
					auto valPtr = (*m_array)[idx];
					oss << valPtr->serializeFormatted(indentLevel + 1);
					first = false;
				}
				oss << "\n" << indent << "]";
			}
			break;
		}
		case JSON_OBJECT: {
			if (m_object->empty()) {
				oss << "{}";
			} else {
				oss << "{\n" << nextIndent;
				bool first = true;
				for (std::map<std::string, JsonObject*>::const_iterator it = m_object->begin(); it != m_object->end(); ++it) {
					if (!first) oss << ",\n" << nextIndent;
					std::string key = it->first;
					auto valPtr = it->second;
					oss << "\"" << key << "\": " << valPtr->serializeFormatted(indentLevel + 1);
					first = false;
				}
				oss << "\n" << indent << "}";
			}
			break;
		}
	}
	return oss.str();
}

std::string Teg::Json::JsonObject::serialize() const {
	std::ostringstream oss;
	switch (m_type) {
		case JSON_NULL: oss << "null"; break;
		case JSON_BOOL: oss << (m_bool ? "true" : "false"); break;
		case JSON_NUMBER: oss << m_number; break;
		case JSON_STRING: {
			oss << "\"";
			for (std::string::const_iterator it = m_string->begin(); it != m_string->end(); ++it) {
				switch (*it) {
					case '\"': oss << "\\\""; break;
					case '\\': oss << "\\\\"; break;
					case '\b': oss << "\\b"; break;
					case '\f': oss << "\\f"; break;
					case '\n': oss << "\\n"; break;
					case '\r': oss << "\\r"; break;
					case '\t': oss << "\\t"; break;
					default: oss << *it;
				}
			}
			oss << "\"";
			break;
		}
		case JSON_ARRAY: {
			oss << "[";
			bool first = true;
			for (int idx = 0; idx < m_array->size(); ++idx) {
				if (!first) oss << ",";
				auto valPtr = (*m_array)[idx];
				oss << valPtr->serialize();
				first = false;
			}
			oss << "]";
			break;
		}
		case JSON_OBJECT: {
			oss << "{";
			bool first = true;
			for (std::map<std::string, JsonObject*>::const_iterator it = m_object->begin(); it != m_object->end(); ++it) {
				if (!first) oss << ",";
				std::string key = it->first;
				auto valPtr = it->second;
				oss << "\"" << key << "\":" << valPtr->serialize();
				first = false;
			}
			oss << "}";
			break;
		}
	}
	return oss.str();
}
