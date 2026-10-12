#include "Student.h"

void StudentDB::load(const std::vector<StudentInfo>& loaded_data) {
    info = loaded_data;
}

int StudentDB::numWidth() const{
    int width =
        static_cast<int>(std::to_string(info.size()).length());
    return width;
}

std::vector<int> StudentDB::getWidthVec() const {
    return { numWidth(), NAME_WIDTH, GROUP_WIDTH, PASS_WIDTH, DIGIT_WIDTH };
}

unsigned int StudentDB::getRow(size_t i) const {
    return (i < info.size()) ? info[i].row : 0;
}

std::string StudentDB::getField(size_t i, Inf field) const {
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

std::string StudentDB::getField(size_t row, size_t col) const {
    Inf field;
    switch (col) {
    case 1: field = Inf::name; break;
    case 2: field = Inf::group; break;
    case 3: field = Inf::pass; break;
    case 4: field = Inf::num; break;
    default: field = Inf::row; break;
    }
    return getField(row, field);
}

std::vector<StudentInfo> StudentDB::getInfoVec() const {
    return info;
}
