#pragma once
#include <iostream>
#include <vector>
#include <string>

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
	const int INFO_COL_COUNT = 4; // Общее число полей в структуре StudentInfo
	
	void load(const std::vector<StudentInfo>& loaded_data) { // метод загрузки данных из файла в базу
        info = loaded_data;
    }

    unsigned int getRow(size_t i) const {
        return (i < info.size()) ? info[i].row : 0;
    }
    std::string getName(size_t i) const {
        return (i < info.size()) ? info[i].name : "";
    }
    std::string getGroup(size_t i) const {
        return (i < info.size()) ? info[i].group : "";
    }
    std::string getPass(size_t i) const {
        return (i < info.size()) ? info[i].pass : "";
    }
    std::string getNum(size_t i) const {
        return (i < info.size()) ? info[i].num : "";
    }
	std::string getField(size_t i, Inf field) const {
		if (i >= info.size()) return "";
		switch (field) {
		case Inf::num: return std::to_string(info[i].row);
		case Inf::name: return info[i].name;
		case Inf::group: return info[i].group;
		case Inf::pass: return info[i].pass;
		case Inf::digit: return info[i].num;
		default: return "";
		}
	}
    std::string getField(size_t row, size_t col) const {
        Inf field;
        switch (col) {
        case 0: field = Inf::name; break;
        case 1: field = Inf::group; break;
        case 2: field = Inf::pass; break;
		case 3: field = Inf::digit; break; 
        default: field = Inf::num; break;
        }
        return getField(row, field);
    }

    size_t size() const {
        return info.size();
    }
    bool empty() const {
        return info.empty();
    }
};