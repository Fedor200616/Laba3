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
    PathType type() const {
        if (file_type != PathType::NONE)
            return file_type;
        if (fs::is_directory(path)) {
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
    std::string typeEmoji() const { //Получение эмодзи на основе типа
        switch (file_type) {
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
    std::string checkEmoji(bool checked = false) const { // Получение эмодзи, что файл соответствует формату
        if (checked)
            return "✅";
        else
            return "";
    }
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

    File(fs::path file) : filePath(file){ //Старый конструктор, остался для совместимости с аргументами cmd
        LOG_INFO("Запуск конструктора File");
        LOG_INFO(std::string("Путь к файлу ") + pathToUtf8(filePath));
        if (filePath.extension() == ".txt")
            type = PathType::TXT;
        else if (filePath.extension() == ".csv")
            type = PathType::CSV;
        else
            LOG_ERROR("Ошибка типа файла: расширение файла " + pathToUtf8(filePath.extension()));
    }
    File(const DirectoryEntry& de) : filePath(de.path), type(de.type()) {
        if (filePath.empty())
            return;
        LOG_INFO("Запуск конструктора File");
        LOG_INFO(std::string("Путь к файлу ") + pathToUtf8(filePath));
        if (!(type == PathType::CSV || type == PathType::TXT))
            LOG_ERROR("Ошибка типа файла: " + std::to_string(static_cast<int>(type)) + ", Расширение файла " + pathToUtf8(filePath.extension()));
    }

    std::vector<StudentInfo> loadFromFile();
    fs::path getFilePath() const {return filePath;}
    bool exist() const { return !filePath.empty(); }
    
    bool isModified() const {
        return is_modified;
    }
};

class Explorer {
private:
    const fs::path default_path; // Путь самого первого нахождения окна
    fs::path curr_path; //Путь, по которому находится окно проводника
    //DirectoryEntry file_path; //Путь к последнему выбранному файлу, и соответсвенно, файлу с которым ма работаем
    std::vector<DirectoryEntry> file_arr;
    void fillFiles();
    bool openDir(const DirectoryEntry& entry);
    bool backDir(const DirectoryEntry& entry);
    bool openFile(const DirectoryEntry& entry);
    
public:
    Explorer(fs::path def_path) : default_path(def_path) {
        curr_path = fs::is_directory(default_path) ? default_path : default_path.parent_path();
        fillFiles();
    };
    std::vector<DirectoryEntry> fileArr() const;
    bool openPath(const DirectoryEntry& entry);

};