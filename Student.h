#pragma once
#include <vector>
#include <string>

// Структуры и enum остаются в заголовочном файле
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

class StudentDB {
private:
    std::vector<StudentInfo> info;

public:
    //Константы
    const int NAME_WIDTH = 20;
    const int GROUP_WIDTH = 12;
    const int PASS_WIDTH = 20;
    const int NUM_WIDTH = 5;
    const int INFO_COL_COUNT = 4;

    //Загрузка вектора информации из файла
    void load(const std::vector<StudentInfo>& loaded_data);

    //Геттеры
    unsigned int getRow(size_t i) const;
    std::string getName(size_t i) const;
    std::string getGroup(size_t i) const;
    std::string getPass(size_t i) const;
    std::string getNum(size_t i) const;
    std::string getField(size_t i, Inf field) const;
    std::string getField(size_t row, size_t col) const;
    std::vector<StudentInfo> getInfoVec() const;

    //Изменение данных


    // Геттеры информации о базе
    size_t size() const { return info.size(); }
    bool empty() const { return info.empty(); }
};