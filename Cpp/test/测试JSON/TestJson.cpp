#include "JsonObject.h"
#include <iostream>

using namespace Teg::Json;

int main() {
	// 测试基本类型
	JsonObject nullValue;
	JsonObject boolValue(true);
	JsonObject numValue(3.14);
	JsonObject strValue("Hello, JSON!");

	std::cout << "Basic types:" << std::endl;
	std::cout << "null: " << nullValue.serialize() << std::endl;
	std::cout << "bool: " << boolValue.serialize() << std::endl;
	std::cout << "number: " << numValue.serialize() << std::endl;
	std::cout << "string: " << strValue.serialize() << std::endl;

	// 测试动态添加对象属性
	JsonObject person;
	person.add("name", JsonObject("John Doe"));
	person.add("age", 30);
	person.add("isStudent", JsonObject(false));

	std::cout << "\nPerson object:" << std::endl;
	std::cout << person.serialize() << std::endl;

	// 测试动态添加数组元素
	JsonObject colors;
	colors.add(JsonObject("red"));
	colors.add(JsonObject("green"));
	colors.add(JsonObject("blue"));

	std::cout << "\nColors array:" << std::endl;
	std::cout << colors.serialize() << std::endl;

	// 测试嵌套结构
	JsonObject course;
	course.add("name", JsonObject("Computer Science"));
	course.add("credits", 4);

	JsonObject student;
	student.add("name", JsonObject("Alice"));
	student.add("age", 22);

	// 动态添加课程到数组
	JsonObject courses;
	courses.add(course);
	// 另一个课程
	JsonObject anotherCourse;
	anotherCourse.add("name", JsonObject("Mathematics"));
	anotherCourse.add("credits", 3);
	courses.add(anotherCourse);
	// 将课程信息添加至学生信息
	student.add("courses", courses);

	std::cout << "\nNested student object:" << std::endl;
	std::cout << student.serialize() << std::endl;
	
	// 测试文件输出功能
	std::cout << "\nTesting file output..." << std::endl;
	std::string jsonFilePath = "test_output/test.json";
	student.serializeToFile(jsonFilePath);
	std::cout << "JSON written to: " << jsonFilePath << std::endl;

	// 测试格式化输出
	std::cout << "\nTesting formatted output:" << std::endl;
	std::cout << student.serializeFormatted() << std::endl;

	// 测试数组到CSV转换
	std::cout << "\nTesting array to CSV..." << std::endl;
	
	// 测试简单数组
	JsonObject simpleArray;
	simpleArray.add(1);
	simpleArray.add(2);
	simpleArray.add(3);
	std::string simpleCsvPath = "test_output/simple.csv";
	simpleArray.serializeToCSV(simpleCsvPath);
	std::cout << "Simple array CSV written to: " << simpleCsvPath << std::endl;

	// 测试嵌套数组
	JsonObject nestedArray;
	JsonObject row1;
	row1.add(1);
	row1.add("A");
	row1.add(JsonObject(true));
	nestedArray.add(row1);
	
	JsonObject row2;
	row2.add(2);
	row2.add("B");
	row2.add(JsonObject(false));
	nestedArray.add(row2);
	
	std::string nestedCsvPath = "test_output/nested.csv";
	nestedArray.serializeToCSV(nestedCsvPath);
	std::cout << "Nested array CSV written to: " << nestedCsvPath << std::endl;

	// 测试错误处理
	try {
		JsonObject notAnArray;
		notAnArray.add("key", "value");
		notAnArray.serializeToCSV("test_output/should_fail.csv");
	} catch (const std::exception& e) {
		std::cout << "\nError handling test passed: " << e.what() << std::endl;
	}

	std::cout << "\nAll tests completed. Check output files in test_output/ directory." << std::endl;
	return 0;
}
