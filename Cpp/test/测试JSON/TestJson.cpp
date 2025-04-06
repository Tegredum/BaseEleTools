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
    
    return 0;
}
