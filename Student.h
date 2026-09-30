#pragma once
#include <iostream>

//Запись о студенте
struct StudentInfo {
    unsigned int i;
    std::string name;
    std::string group;
    std::string pass;
    std::string num;
};

enum class Inf {
    num,
    name,
    group,
    pass,
    digit
};

/// <summary>
/// Класс работы с базой студентов
/// </summary>
class StudentDB {
private:
    std::vector<StudentInfo> info;

public:
};