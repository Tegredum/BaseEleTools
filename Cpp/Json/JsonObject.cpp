#include "JsonObject.h"
#include <sstream>
#include <stdexcept>

namespace Teg {
namespace Json {

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

} // namespace Json
} // namespace Teg

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
