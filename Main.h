#pragma once
#include <filesystem>
#include <iostream>
#include "Student.h"
#include "Log.h"

namespace fs = std::filesystem;

struct AppConfig {
    fs::path filePath; 
    bool testMode = false;
    bool do_log = false;
	std::string arg_str; // Строка с аргументами командной строки

    static AppConfig parseArgs(int argc, char* argv[]) {
        AppConfig config;

        for (int i = 0; i < argc; ++i) {
            if (i > 0)
                config.arg_str += ' ';

            config.arg_str += argv[i];
        }

        for (int i = 1; i < argc; ++i) {
            std::string_view arg(argv[i]);

            if (arg == "-f" && i + 1 < argc) {
                config.filePath = argv[++i];
            }
            else if (arg == "-l") {
                config.do_log = true;
            }
            else if (arg == "--test") {
                config.testMode = true;
            }
        }

        return config;
    }
};

/// <summary>
/// Класс информации о программе
/// </summary>
class Info {
private:
    fs::path exe_filepath; //Путь к exe файлу программы
    bool initExePath(int argc, char* argv[]) {
        if (argc > 0 && argv && argv[0]) {
            exe_filepath = fs::absolute(argv[0]);
            LOG_INFO("Путь к exe файлу: " + pathToUtf8(exe_filepath));
            return true;
        }
        return false;
    }
    AppConfig config;


public:
    Info(int argc, char* argv[]) {
        if (!initExePath(argc, argv)) {
            exe_filepath = fs::current_path();
            LOG_ERROR("Ошибка нахождения пути к exe, выбран путь по умолчанию " + pathToUtf8(exe_filepath));
        }
        config = AppConfig::parseArgs(argc, argv);
    }

    fs::path getStartPath() const {
        return config.filePath;
    }
    fs::path getExe() const {
        return exe_filepath;
    }
    fs::path getExeDirectory() const {
        return exe_filepath.parent_path();
    }
    AppConfig getConfig(){
        return config;
    }

};

