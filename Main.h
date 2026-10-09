#pragma once
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

#include "Student.h"
#include "Log.h"

namespace fs = std::filesystem;

struct AppConfig {
    fs::path filePath;
    bool testMode = false;
    bool do_log = false;
    std::string arg_str;

    static AppConfig parseArgs(int argc, char* argv[]) {
        AppConfig config;

        // Сохраняем аргументы для логирования
        for (int i = 0; i < argc; ++i) {
            if (i > 0)
                config.arg_str += ' ';

            config.arg_str += argv[i];
        }

        // Разбираем аргументы
        for (int i = 1; i < argc; ++i) {
            std::string_view arg(argv[i]);

            if (arg == "-f") {
                if (i + 1 >= argc) {
                    LOG_ERROR("После -f необходимо указать путь к файлу.");
                    continue;
                }

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

class Info {
private:
    fs::path exe_filepath;
    AppConfig config;

    bool initExePath(int argc, char* argv[]) {
        if (argc > 0 && argv && argv[0]) {
            exe_filepath = fs::absolute(argv[0]);
            LOG_INFO("Путь к exe файлу: " + pathToUtf8(exe_filepath));
            return true;
        }

        return false;
    }

public:
    Info(int argc, char* argv[]) {
        LOG_INFO("Запуск конструктора Info");

        if (!initExePath(argc, argv)) {
            exe_filepath = fs::current_path();
            LOG_ERROR(
                "Ошибка нахождения пути к exe, выбран путь по умолчанию: "
                + pathToUtf8(exe_filepath)
            );
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

    AppConfig getConfig() const {
        return config;
    }
};