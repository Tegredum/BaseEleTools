#ifndef TEG_JSON_OBJECT_H
#define TEG_JSON_OBJECT_H

#include <string>
#include <vector>
#include <map>

namespace Teg {
namespace Json {

class JsonObject {
public:
	// 基本类型枚举
	enum Type {
		JSON_NULL,
		JSON_BOOL,
		JSON_NUMBER,
		JSON_STRING,
		JSON_ARRAY,
		JSON_OBJECT
	};

	// 构造函数
	JsonObject();
	explicit JsonObject(const std::string& str);
	JsonObject(const char str[]) : JsonObject(std::string(str)) {}
	explicit JsonObject(double num);
	explicit JsonObject(bool b);
	JsonObject(const JsonObject& other);
	~JsonObject();

	// 类型检查
	Type type() const;

	// 值获取
	bool toBool() const;
	double toNumber() const;
	std::string toString() const;
	std::vector<JsonObject> toArray() const;
	std::map<std::string, JsonObject> toObject() const;

	// 动态添加元素
	void add(const std::string& key, const JsonObject& value);
	void add(const char key[], const JsonObject& value) {
		this->add(std::string(key), value);
	}
	void add(const char key[], double value) {
		this->add(std::string(key), JsonObject(double(value)));
	}
	void add(const JsonObject& value); // 用于数组
	void add(double value) {
		this->add(JsonObject(double(value)));
	}

	// 序列化为字符串
	std::string serialize() const;
	
	// 序列化到文件
	void serializeToFile(const std::string& filePath) const;
	
	// 格式化序列化(带缩进和换行)
	std::string serializeFormatted(int indentLevel = 0) const;
	
	// 将数组序列化为CSV文件
	void serializeToCSV(const std::string& filePath) const;

private:
	Type m_type;
	// 辅助方法: 递归创建目录
	static bool createDirectory(const std::string& path);
	union {
		bool m_bool;
		double m_number;
		std::string* m_string;
		std::vector<JsonObject*>* m_array;
		std::map<std::string, JsonObject*>* m_object;
	};
}; // class JsonObject

} // namespace Json
} // namespace Teg

#endif // TEG_JSON_OBJECT_H
