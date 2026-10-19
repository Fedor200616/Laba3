#pragma once
#include <vector>
#include <string>
#include "Log.h"

// Структуры и enum остаются в заголовочном файле
struct StudentInfo {
    unsigned int row;
    std::string name;
    std::string group;
    std::string pass;
    std::string num;
};

enum class Inf {
    row,
    name,
    group,
    pass,
    num
};

namespace validate {
    // Проверка символов
    inline bool isLatinLower(wchar_t c);
    inline bool isLatinUpper(wchar_t c);
    inline bool isCyrLower(wchar_t c);
    inline bool isCyrUpper(wchar_t c);
    inline bool isDigit(wchar_t c);
    inline bool isPunct(wchar_t c);

    // Проверка полей
    bool name(const std::string& s);
    bool group(const std::string& s);
    bool groupNum(const std::string& s);
    bool password(const std::string& s);

    // Проверка поля по его типу
    bool check(Inf field, const std::string& s);
}



class DataBase { //Следует сменить на просто DataBase
private:
    std::vector<StudentInfo> info;
public:
    //Константы
    const int NAME_WIDTH = 20; // Длина поля имени
    const int GROUP_WIDTH = 12; // длина поля группы
    const int PASS_WIDTH = 20; // длина поля пароля
    const int DIGIT_WIDTH = 5; // Длина поля номера в группе
    const int INFO_COL_COUNT = 5; // Количество полей в таблице
    const std::vector<std::string> HEADER = {
        "#", "Имя", "Группа", "Пароль", "Номер"
    };

    //Загрузка вектора информации из файла
    void load(const std::vector<StudentInfo>& loaded_data);

    //Геттеры
    int numWidth() const;
    std::vector<int> getWidthVec() const;
    unsigned int getRow(size_t i) const;
    std::string getField(size_t i, Inf field) const;
    std::string getField(size_t row, size_t col) const;
    std::vector<StudentInfo> getInfoVec() const;
    std::vector<size_t> getFilterHit(size_t col, std::string str) const;

    // Геттеры информации о базе
    size_t size() const { return info.size(); }
    bool empty() const { return info.empty(); }

    //Изменение данных
    void updateNum();
    void setField(size_t row, size_t col, std::string str = "");
    void setField(size_t row, Inf col, std::string str = "");
    void removeRow(size_t row);
    void insertRow(size_t row = -1);
    void editAllFind(size_t col, std::string str, std::string new_str);

};