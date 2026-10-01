#pragma once
#include <iostream>

//Запись о студенте
struct StudentInfo {
    unsigned int row;
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
    const int NAME_WIDTH = 20;
    const int GROUP_WIDTH = 12;
    const int PASS_WIDTH = 20;
    const int NUM_WIDTH = 5;
	const int INFO_COL_COUNT = 5; // Общее число полей в структуре StudentInfo
	
    int getRow(int i) {
        return info[i].row;
    }
    std::string getName(int i) {
        return info[i].name;
    }
    std::string getGroup(int i) {
        return info[i].group;
    }
    std::string getPass(int i) {
        return info[i].pass;
    }
    std::string getNum(int i) {
        return info[i].num;
    }

    int size() const {
		return info.size();
	}
};