#include "Student.h"
static size_t infToSize(Inf inf) {
    switch (inf) {
    case Inf::row: return 0;
    case Inf::name: return 1;
    case Inf::group: return 2;
    case Inf::pass: return 3;
    case Inf::num: return 4;

    default: return -1;
    }
}
static Inf sizeToInf(size_t col) {
    switch (col) {
    case 1: return Inf::name;
    case 2: return Inf::group;
    case 3: return Inf::pass;
    case 4: return Inf::num;
    default: return Inf::row;
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

void DataBase::updateNum() {
    for (int i = 0; i < size(); i++) {
        info[i].row = i + 1;
    }
}

void DataBase::setField(size_t row, Inf col, std::string str) {
    LOG_INFO("Поле изменено: строка " + std::to_string(row) +
        " колонка " + std::to_string(infToSize(col)) +
        " старое поле " + getField(row, col) +
        " новое поле " + str);
    if (row >= info.size()) return ;
    switch (col) {
    case Inf::name: info[row].name = str; break;
    case Inf::group: info[row].group = str; break;
    case Inf::pass: info[row].pass = str; break;
    case Inf::num: info[row].num = str; break;
    default: return;
    }
}
void DataBase::setField(size_t row, size_t col, std::string str) {
    setField(row, sizeToInf(col), str);
}

void DataBase::removeRow(size_t row) {
    if (row >= info.size()) {
        LOG_WARN("Попытка удалить несуществующую строку");
        return;                 // пустая база или неверный индекс
    }
    LOG_INFO("Удаление строки: " + std::to_string(row) + ", фамилия: " + info[row].name);
    info.erase(info.begin() + row);
    updateNum();
}

void DataBase::insertRow(size_t row) {
    if (row > info.size() || row < 0) row = info.size();       // npos и лишние индексы → в конец

    // Референсная строка: та, перед которой вставляем (если она есть)
    std::string ref = (row < info.size()) ? info[row].name : "(конец списка)";
    LOG_INFO("Добавление строки под номером: " + std::to_string(row) + ", фамилия референс: " + ref);

    info.insert(info.begin() + row, StudentInfo{ 0, "", "", "", "" });
    updateNum();
}
