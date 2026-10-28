#include "Student.h"
#include "Input.h"
#include <cstdint>
#include <string>
#include <cctype>
size_t infToSize(Inf inf) {
    switch (inf) {
    case Inf::row: return 0;
    case Inf::name: return 1;
    case Inf::group: return 2;
    case Inf::pass: return 3;
    case Inf::num: return 4;

    default: return -1;
    }
}
Inf sizeToInf(size_t col) {
    switch (col) {
    case 1: return Inf::name;
    case 2: return Inf::group;
    case 3: return Inf::pass;
    case 4: return Inf::num;
    default: return Inf::row;
    }
}

static bool search_field(const std::string& data, const std::string& search) {
    if (search.size() > data.size()) {
        return 0;
    }
    for (int i = 0; i < search.size(); i++) {
        if (!(search[i] == data[i])) {
            return 0;
        }
    }
    return 1;
}



namespace validate {
    using namespace Encoding;
    namespace {
        // Допустимые символы
        constexpr wchar_t LATIN_LO = L'a', LATIN_HI = L'z';
        constexpr wchar_t LATIN_CAP_LO = L'A', LATIN_CAP_HI = L'Z';
        constexpr wchar_t CYR_LO = 0x0430, CYR_HI = 0x044F;
        constexpr wchar_t CYR_CAP_LO = 0x0410, CYR_CAP_HI = 0x042F;
        constexpr wchar_t YO = 0x0451, YO_CAP = 0x0401;
        constexpr wchar_t DIGIT_LO = L'0', DIGIT_HI = L'9';

        constexpr std::size_t PASS_MIN = 8;
        constexpr std::size_t PASS_MAX = 20;

        // Проверка правильности UTF-8
        bool validUtf8(const std::string& s) {
            return toUtf8(fromUtf8(s)) == s;
        }

        // Проверка любой латинской или русской буквы
        bool isLetter(wchar_t c) {
            return isLatinLower(c) || isLatinUpper(c) ||
                isCyrLower(c) || isCyrUpper(c);
        }

    }

    // ---------------- Проверка символов ----------------

    // Проверка маленькой латинской буквы
    inline bool isLatinLower(wchar_t c) {
        return c >= LATIN_LO && c <= LATIN_HI;
    }

    // Проверка заглавной латинской буквы
    inline bool isLatinUpper(wchar_t c) {
        return c >= LATIN_CAP_LO && c <= LATIN_CAP_HI;
    }

    // Проверка маленькой русской буквы
    inline bool isCyrLower(wchar_t c) {
        return (c >= CYR_LO && c <= CYR_HI) || c == YO;
    }

    // Проверка заглавной русской буквы
    inline bool isCyrUpper(wchar_t c) {
        return (c >= CYR_CAP_LO && c <= CYR_CAP_HI) || c == YO_CAP;
    }

    // Проверка цифры
    inline bool isDigit(wchar_t c) {
        return c >= DIGIT_LO && c <= DIGIT_HI;
    }

    // Проверка специального знака ASCII
    inline bool isPunct(unsigned char c) {
        return std::ispunct(static_cast<unsigned char>(c));
    }

    bool noData(const std::string& s){
        return s == vector_nodata;
    }
    
    bool name(const std::string& s) { //Проверка фамилии
        if (s == "0") return true; // Выход в меню

        if (!validUtf8(s)) return false;
        std::wstring w = fromUtf8(s);

        if (noData(s))
            return true;

        if (w.empty() || !isLatinUpper(w[0]) && !isCyrUpper(w[0]))
            return false;
        
        // После первой буквы разрешены маленькие буквы и знаки -, _, '
        for (std::size_t i = 1; i < w.size(); ++i) {
            wchar_t c = w[i];

            if (!(isLatinLower(c) || isCyrLower(c) ||
                c == L'-' || c == L'_' || c == L'\''))
                return false;
        }

        return true;
    }

    bool group(const std::string& s) { // Проверка группы
        if (s == "_" || s == "0") return true; // Пустое поле или выход

        if (!validUtf8(s)) return false;
        std::wstring w = fromUtf8(s);

        if (noData(s))
            return true;

        if (w.empty()) return false;

        // Разрешены буквы, цифры, дефис и подчёркивание
        for (wchar_t c : w) {
            if (!(isLetter(c) || isDigit(c) ||
                c == L'-' || c == L'_'))
                return false;
        }

        return true;
    }

    bool groupNum(const std::string& s) { // Проверка номера ученика
        if (s == "_" || s == "0") return true; // Пустое поле или выход

        if (!validUtf8(s)) return false;
        std::wstring w = fromUtf8(s);

        if (noData(s))
            return true;

        if (w.empty() || w[0] == L'-') return false;

        // Номер должен состоять только из цифр
        for (wchar_t c : w) {
            if (!isDigit(c)) return false;
        }

        return true;
    }

    bool password(const std::string& s) { // Проверка пароля
        if (s == "0") return true; // Выход в меню

        if (!validUtf8(s)) return false;
        std::wstring w = fromUtf8(s);

        if (noData(s))
            return true;

        // Проверка длины пароля
        if (w.size() < PASS_MIN || w.size() >= PASS_MAX)
            return false;

        bool capital = false;
        bool lower = false;
        bool digit = false;
        bool special = false;

        // Проверка состава пароля
        for (wchar_t c : w) {
            if (isLatinUpper(c))
                capital = true;
            else if (isLatinLower(c))
                lower = true;
            else if (isDigit(c))
                digit = true;
            else if (isPunct(c))
                special = true;
            else
                return false; // Русские буквы, пробелы и другие символы
        }

        return capital && lower && digit && special;
    }

    bool check(Inf field, const std::string& s) { // Проверка по типу поля
        if (s.empty()) {
            return true;
        }
        switch (field) {
        case Inf::name:  return name(s);
        case Inf::group: return group(s);
        case Inf::num:   return groupNum(s);
        case Inf::pass:  return password(s);
        default:         
            LOG_ERROR("Неизвестный тип поля");
            return false;
        }
    }
}

void DataBase::load(const std::vector<StudentInfo>& loaded_data) {
    info = loaded_data;
}

int DataBase::numWidth() const{
    int width =
        static_cast<int>(std::to_string(info.size()).length());
    return width;
}

std::vector<int> DataBase::getWidthVec() const {
    return { numWidth(), NAME_WIDTH, GROUP_WIDTH, PASS_WIDTH, DIGIT_WIDTH };
}

unsigned int DataBase::getRow(size_t i) const {
    return (i < info.size()) ? info[i].row : 0;
}

std::string DataBase::getField(size_t i, Inf field) const {
    if (i >= info.size()) return "";
    switch (field) {
    case Inf::row: return std::to_string(info[i].row);
    case Inf::name: return info[i].name;
    case Inf::group: return info[i].group;
    case Inf::pass: return info[i].pass;
    case Inf::num: return info[i].num;
    default: return "";
    }
}

std::string DataBase::getField(size_t row, size_t col) const {
    return getField(row, sizeToInf(col));
}

std::vector<StudentInfo> DataBase::getInfoVec() const {
    return info;
}

std::vector<size_t> DataBase::getFilterHit(size_t col, std::string str) const {
    std::vector<size_t> filtered = {};
    for (size_t i = 0; i < size(); i++) {
        if (search_field(getField(i, col), str)) {
            filtered.push_back(i);
        }
    }
    return filtered;
}

std::vector<size_t> DataBase::getFilterHit() const {
    return getFilterHit(filter_col, filter_token);
}

bool DataBase::inFilter(size_t row) const {
    if (!show_filtered) return true;
    if (row >= size()) return false;
    return search_field(getField(row, filter_col), filter_token);
}

bool DataBase::isOnfilter() const {
    return show_filtered;
}

std::string DataBase::getFilterInf() const {
    if (filter_col == 0)
        return "";
    else
        return filter_token + ", в колонке " + HEADER[filter_col];
}

void DataBase::updateNum() {
    for (int i = 0; i < size(); i++) {
        info[i].row = i + 1;
    }
}

/// <summary>
/// Сеттер ввода в поле
/// </summary>
/// <param name="row">индекс строки поля</param>
/// <param name="col">тип колонки ввода</param>
/// <param name="str">вводимая строка</param>
/// <returns>0 если ввод неверен, 1 - если верно</returns>
bool DataBase::setField(size_t row, Inf col, std::string str) {
    LOG_INFO("Поле изменено: строка " + std::to_string(row) +
        " колонка " + std::to_string(infToSize(col)) +
        " старое поле " + getField(row, col) +
        " новое поле " + str);
    if (row >= info.size()) {
        LOG_ERROR("Выход за границы массива базы данных");
        return 0;
    }
    if (str.empty() <= 0){
        std = vector_nodata;
    }
    if (!validate::check(col, str)) {
        LOG_ERROR("Неверные символы в поле, поле: " + str);
        return 0;
    }
    switch (col) {
    case Inf::name: 
        info[row].name = str;
        break;
    case Inf::group:
        info[row].group = str; 
        break;
    case Inf::pass: 
        info[row].pass = str; 
        break;
    case Inf::num: 
        info[row].num = str; 
        break;
    default: 
        LOG_ERROR("Ошбика ввода, неизвестный тип колонки.");
        return 0;
    }
    return 1;
}
/// <summary>
/// Сеттер ввода в поле
/// </summary>
/// <param name="row">индекс строки поля</param>
/// <param name="col">тип колонки ввода</param>
/// <param name="str">вводимая строка</param>
/// <returns>0 если ввод неверен, 1 - если верно</returns>
bool DataBase::setField(size_t row, size_t col, std::string str) {
    return setField(row, sizeToInf(col), str);
}

void DataBase::removeRow(size_t row) {
    if (row >= info.size()) {
        LOG_WARN("Попытка удалить несуществующую строку");
        return;                 // пустая база или неверный индекс
    }
    LOG_INFO("Удаление строки: " + std::to_string(row) + ", фамилия: " + info[row].name);
    if (size() <= 1) { // если после удаления база будет пустой
        insertRow(1); // Дабвляем хотя бы одну строку
    }
    info.erase(info.begin() + row);
    updateNum();
}

void DataBase::insertRow(size_t row) {
    if (row > info.size()) row = info.size(); // Если указанная строка ввода больше массива, то это просто последняя строка

    // Референсная строка: та, перед которой вставляем (если она есть)
    std::string ref = (row < info.size()) ? info[row].name : "(конец списка)";
    LOG_INFO("Добавление строки под номером: " + std::to_string(row) + ", фамилия референс: " + ref);

    info.insert(info.begin() + row, StudentInfo{ 0, "", "", "", "" });
    updateNum();
}

/// <summary>
/// Функция изменения всех строк по поиску
/// </summary>
/// <param name="col">колонка для изменения</param>
/// <param name="str">строка поиска</param>
/// <param name="new_str">новая строка</param>
/// <returns>0 если ввод неверен, 1 - если верно</returns>
bool DataBase::editAllFind(size_t col, std::string str, std::string new_str) {
    if (!validate::check(sizeToInf(col), str)) {
        LOG_ERROR("Неверные символы в поле, поле: " + str);
        return 0;
    }
    std::vector<size_t> filt_row_arr = getFilterHit(col, str); //Массив индексов подпадающих под поиск
    for (int i = 0; i < filt_row_arr.size(); i++) {
        setField(filt_row_arr[i], col, new_str);
    }
    return 1;
}

void DataBase::changeFilter() {
    show_filtered = !show_filtered;
}

void DataBase::changeFilter(bool b) {
    show_filtered = b;
}

void DataBase::changeFilter(size_t col, std::string str, bool b) {
    filter_token = str;
    filter_col = col;
    show_filtered = b;
}

size_t DataBase::checkIntegrity() {
    data_valid = true;

    for (size_t i = 0; i < info.size(); ++i) {
        const StudentInfo& student = info[i];

        // Проверяем нумерацию строк
        if (student.row != i + 1) {
            data_valid = false;
            LOG_ERROR("Нарушена нумерация строк: " +
                      std::to_string(i));
            return i;
        }

        // Проверяем корректность значений полей
        if (!validate::check(Inf::name, student.name) ||
            !validate::check(Inf::group, student.group) ||
            !validate::check(Inf::num, student.num) ||
            !validate::check(Inf::pass, student.pass)) {

            data_valid = false;
            LOG_ERROR("Обнаружены некорректные данные в строке: " +
                      std::to_string(i + 1));
            return i;
        }
    }

    LOG_INFO("Проверка целостности базы данных пройдена");
    return info.size();
}