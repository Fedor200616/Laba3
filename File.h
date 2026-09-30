#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

#include "Student.h"
#include "Log.h"

namespace fs = std::filesystem;

enum class fileType {
    NONE,
    TXT,
    CSV,
    ERROR
};

/// <summary>
/// Отвечает за работу с файлом, переносы в него и из него массивов
/// </summary>
class File {
private:
    fs::path filePath = ""; // путь к файлу
    fileType type = fileType::NONE; // тип файла
    bool is_modified = false; // Был ли модифицирован файл

    StudentInfo copyFromString(const std::string& str_buf, unsigned int i);
public:
    bool have_marker = false;

    File(fs::path file){
        filePath = file;
        LOG_INFO("Путь к файлу " + filePath)
    }
    File() {}

    std::vector<StudentInfo> loadFromFile();
    bool saveToFile(const std::vector<StudentInfo>& Info);
    bool createBackup(const fs::path& main_path, const std::vector<StudentInfo>& Info);
    bool removeBackup(const fs::path& main_path);
    

    bool isModified() const {
        return is_modified;
    }
};

