#include "Student.h"

void StudentDB::load(const std::vector<StudentInfo>& loaded_data) {
    info = loaded_data;
}

unsigned int StudentDB::getRow(size_t i) const {
    return (i < info.size()) ? info[i].row : 0;
}

std::string StudentDB::getName(size_t i) const {
    return (i < info.size()) ? info[i].name : "";
}

std::string StudentDB::getGroup(size_t i) const {
    return (i < info.size()) ? info[i].group : "";
}

std::string StudentDB::getPass(size_t i) const {
    return (i < info.size()) ? info[i].pass : "";
}

std::string StudentDB::getNum(size_t i) const {
    return (i < info.size()) ? info[i].num : "";
}

std::string StudentDB::getField(size_t i, Inf field) const {
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

std::string StudentDB::getField(size_t row, size_t col) const {
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

std::vector<StudentInfo> StudentDB::getInfoVec (){
    return info;
}
