#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <Windows.h>

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
    HIDE,
    NONE,
    ERR
};

enum class OpenResult {
    DirectoryChanged,
    FileSelected,
    Failed
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
        const DWORD attributes = GetFileAttributesW(path.c_str());

        if (attributes != INVALID_FILE_ATTRIBUTES && //Провека на валидность
            (attributes & FILE_ATTRIBUTE_HIDDEN)) { //проверка что файл скрыт системой
            return PathType::HIDE;
        }
        const std::wstring name = path.filename().wstring();

        if (name.starts_with(L"~$"))
            return PathType::HIDE;
        std::error_code ec;
        if (fs::is_directory(path, ec))
            return PathType::Folder;
        if (ec)
            return PathType::ERR;
        if (!fs::is_regular_file(path, ec))
            return PathType::ERR;
        if (ec)
            return PathType::ERR;
        const auto ext = path.extension();
        if (ext == ".txt") return PathType::TXT;
        if (ext == ".csv") return PathType::CSV;
        return PathType::OtherFile;
    }
    //Геттеры:
    std::string typeEmoji() const { //Получение эмодзи на основе типа
        switch (type()) {
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
            return "↩";
        default:
            return "▫️";
        }
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
    bool exist() const {
        std::error_code ec;
        return !filePath.empty() && fs::is_regular_file(filePath, ec);
    }
    
    bool isModified() const {
        return is_modified;
    }
};

class Explorer {
private:
    const fs::path default_path; // Путь самого первого нахождения окна
    fs::path curr_path; //Путь, по которому находится окно проводника
    DirectoryEntry selected_entry;
    //DirectoryEntry file_path; //Путь к последнему выбранному файлу, и соответсвенно, файлу с которым ма работаем
    std::vector<DirectoryEntry> file_arr;
    void fillFiles();
public:
    Explorer(fs::path def_path) : default_path(def_path), selected_entry(fs::path{}) {
        curr_path = fs::is_directory(default_path) ? default_path : default_path.parent_path();
        fillFiles();
    };
    std::vector<DirectoryEntry> fileArr() const; //вектор DirectoryEntry файлов в директории
    OpenResult openPath(const DirectoryEntry& entry); // открытие пути
    DirectoryEntry getFileEntry() const; // получение информации об файле
    fs::path currentPath() const { return curr_path; }
};