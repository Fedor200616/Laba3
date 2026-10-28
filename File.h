#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

#include "Student.h"
#include "Log.h"

namespace fs = std::filesystem;

enum class PathType {
    Folder,
    TXT,
    CSV,
    OtherFile,
    RootDir,
    BackDir,
    EXE,
    ERR
};

/// <summary>
/// Отвечает за работу с файлом, переносы в него и из него массивов
/// </summary>
class File {
private:
    fs::path filePath = ""; // путь к файлу
    PathType type = PathType::NONE; // тип файла
    bool is_modified = false; // Был ли модифицирован файл

    StudentInfo copyFromString(const std::string& str_buf, unsigned int i, char sep_ch = '|');
public:
    bool have_marker = false;

    File(fs::path file){
        LOG_INFO("Запуск конструктора File");
        filePath = file;
        LOG_INFO(std::string("Путь к файлу ") + pathToUtf8(filePath));
    }
    File() {}

    std::vector<StudentInfo> loadFromFile();
    fs::path getFilePath(){return filePath;};
    bool exist();
    
    bool isModified() const {
        return is_modified;
    }
};

struct DirectoryEntry {
    fs::path path;
    PathType type(){
        if(fs::is_directory(path)){
            return PathType::Folder;
        }
        else if (!fs::is_regular_file(path)) {
            PathType::ERR;
        }
        std::string ext = path.ex

    }   
    std::string name() {
        return path.filename();
    }
    std::string typeEmoji(){
        switch (type){
        case PathType::Folder:
            return "📁";
        case PathType::TXT:
            return "📝";
        case PathType::CSV:
            return "📑";
        case PathType::OtherFile:
            return "📄";
        case PathType::RootDir:
            return "📂";
        case PathType::BackDir:
            return "⬆️";
        case PathType::EXE:
            return "⚙️";
        default:
            return "▫️";
        }
    }
    std::string checkEmoji(bool checked = false){
        if (checked)
            return "✅";
        else
            return "";
    }
};

class Explorer {
private:
    static constexp fs::path exe_filepath;
    DirectoryEntry curr_path = exe_filepath;

}