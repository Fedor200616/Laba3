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
    NONE,
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
    fs::path getFilePath() const {return filePath;}
    bool exist() const { return !filePath.empty(); }
    
    bool isModified() const {
        return is_modified;
    }
};

struct DirectoryEntry {
    fs::path path;
    PathType file_type = PathType::NONE;
    bool real;
    std::uintmax_t size;
    std::filesystem::file_time_type last_write;
    
    DirectoryEntry(fs::path fp) : path(fp) {
        init();
    };
    DirectoryEntry(fs::path f_path, PathType f_type) : path(f_path), file_type(f_type) {
        init();
    };

    void init() {
        real = type() != PathType::NONE &&
            type() != PathType::ERR;
        if (!real) //Если странный файл, то и смысла остального нет
            return;
        std::error_code ec; //Отладка ошибок
        last_write = fs::last_write_time(path, ec);
        if (ec)
            last_write = {};
        if (type() == PathType::Folder) { // Для папки путь берем за 0
            size = 0;
            return;
        }

        size = fs::file_size(path, ec);

        if (ec)
            size = 0;
    }
    PathType type(){
        if (file_type != PathType::NONE)
            return file_type;
        if(fs::is_directory(path)){
            return PathType::Folder;
        }
        else if (!fs::is_regular_file(path)) {
            return PathType::ERR;
        }
        auto ext = path.extension();
        if (ext == ".txt") return PathType::TXT;
        else if (ext == ".csv") return PathType::CSV;
        else return PathType::OtherFile;
    }
    //Геттеры:
    std::string typeEmoji() const{ //Получение эмодзи на основе типа
        switch (file_type){
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
        default:
            return "▫️";
        }
    }
    std::string checkEmoji(bool checked = false) const{ // Получение эмодзи, что файл соответствует формату
        if (checked)
            return "✅";
        else
            return "";
    }
};

class Explorer {
private:
    const fs::path default_path;
    fs::path curr_path;
    std::vector<DirectoryEntry> files;
public:
    explicit Explorer(const fs::path& startPath)
        : curr_path(startPath) {}

};