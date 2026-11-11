#pragma once
#include <vector>
#include <string>
#include "Log.h"

const std::string FILE_NODATA = "Нет данных";
const std::string VECTOR_NODATA = "";

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

enum class ValRes{
    APPLY,

    NO_UTF8,
    EMPTY,
    UNKNOWN,

    NAME_NOT_VER,
    GROUP_NOT_VER,
    NUM_NOT_VER,
    PASS_LENGTH,
    PASS_NOT_APP_WORD,
    PASS_NO_UPPER,
    PASS_NO_LOWER,
    PASS_NO_DIGIT,
    PASS_NO_PUNCT,
};

namespace validate {
    std::string info(const Inf& col);
    //Функция вывода сообщения об ошибке в зависимости от ошибки
    std::string valMessage(ValRes res, const std::string& str = "");
    // Проверка символов
    inline bool isLatinLower(wchar_t c);
    inline bool isLatinUpper(wchar_t c);
    inline bool isCyrLower(wchar_t c);
    inline bool isCyrUpper(wchar_t c);
    inline bool isDigit(wchar_t c);
    inline bool isPunct(unsigned char c);

    // Проверка полей
    bool noData(const std::string& s); // Если поле пустое, то оно все еще корректно
    
    ValRes name(const std::string& s);
    ValRes group(const std::string& s);
    ValRes groupNum(const std::string& s);
    ValRes password(const std::string& s);

    // Проверка поля по его типу
    ValRes check(Inf field, const std::string& s);
    bool bCheck(const Inf &field, const std::string& s);
}

size_t infToSize(Inf inf);
Inf sizeToInf(size_t col);

class DataBase {
private:
    std::vector<StudentInfo> info;  // Все записи базы данных
    std::vector<StudentInfo> backup_info; // копия начального файла до изменений
    bool data_valid = true;

    bool show_filtered = false;     // Включён ли фильтр (показывать только найденные строки)
    size_t filter_col = 1;          // Колонка, по которой работает фильтр
    std::string filter_token;       // Строка поиска фильтра

    bool is_modified = false; // Была ли модифицирована база
    void setModif(bool val = true) { is_modified = val; };
public:
    // Константы
    const int NAME_WIDTH = 20;      // Длина поля имени
    const int GROUP_WIDTH = 12;     // Длина поля группы
    const int PASS_WIDTH = 20;      // Длина поля пароля
    const int DIGIT_WIDTH = 5;      // Длина поля номера в группе
    const int INFO_COL_COUNT = 5;   // Количество полей в таблице
    const std::vector<std::string> HEADER = {
        "#", "Имя", "Группа", "Пароль", "Номер"
    };

    // Загрузка вектора информации из файла (заменяет все текущие данные)
    void load(const std::vector<StudentInfo>& loaded_data);
    void loadBackup();

    // ---------- Геттеры ----------

    // Ширина колонки "#": количество цифр в размере базы
    int numWidth() const;

    // Вектор ширин всех колонок в порядке HEADER
    std::vector<int> getWidthVec() const;

    // Номер строки (поле row) по индексу в векторе; 0, если индекс неверен
    unsigned int getRow(size_t i) const;

    // Значение поля строки i по типу колонки (Inf); "" при выходе за границы
    std::string getField(size_t i, Inf field) const;

    // Значение поля строки row по номеру колонки (0 - #, 1 - имя, ...)
    std::string getField(size_t row, size_t col) const;

    // Копия всего вектора записей
    std::vector<StudentInfo> getInfoVec() const;

    // Индексы строк, у которых колонка col подходит под поиск str
    std::vector<size_t> getFilterHit(size_t col, const std::string &str) const;

    // Индексы строк, подходящих под текущий фильтр (filter_col и filter_token)
    std::vector<size_t> getFilterHit() const;

    // Проходит ли строка row текущий фильтр (true, если фильтр выключен)
    bool inFilter(size_t row) const;

    // Включён ли сейчас фильтр
    bool isOnFilter() const;

    // Формирования строки о текущих строках фильтра
    std::string getFilterInf() const;

    size_t getFilterCol() const { return filter_col; }
    std::string getFilterStr() const { return filter_token; }

    // ---------- Информация о базе ----------

    // Общее количество записей
    size_t size() const { return info.size(); }

    // Количество записей, видимых с учётом фильтра
    size_t sizeIfFilter() const { return show_filtered ? getFilterHit().size() : size(); }

    // Пуста ли база
    bool empty() const { return info.empty(); }

    bool getModif() const { return is_modified; }

    // ---------- Изменение данных ----------

    // Перенумеровать поле row у всех записей (1, 2, 3, ...)
    void updateNum();

    // Удалить строку по индексу
    void removeRow(size_t row);

    // Вставить пустую строку перед индексом row (по умолчанию в конец)
    void insertRow(size_t row = -1);

    // Записать значение в поле по номеру колонки; false, если ввод неверен
    bool setField(size_t row, const Inf &col, const std::string& str = "");

    // Записать значение в поле по типу колонки (Inf); false, если ввод неверен
    bool setField(size_t row, size_t col, const std::string &str = "");

    // Заменить в колонке col значение на new_str у всех строк, найденных по поиску str
    bool editAllFind(size_t col, const std::string &str, const std::string &new_str);

    // Переключить фильтр вкл/выкл
    void changeFilter();
    void changeFilter(bool b);

    // Задать фильтр: колонка, строка поиска, включён ли он
    void changeFilter(size_t col, const std::string &str, bool b = false);

    bool isDataValid() const {
        return data_valid;
    }
};