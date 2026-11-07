#pragma once
#include <filesystem>
#include <string>
#include <string_view>
#include "Input.h"

#include "Log.h"

namespace fs = std::filesystem;

static const std::string HELP_COUT = "Программа редактирования данных о студентах.\n"
    "Используйте аргументы:\n"
    "-l для записи лога о работе \n-f + <полный путь к файлу> для запуска с открытия выбранного файла\n"
    "Нажмите Enter";

struct AppConfig {
    fs::path filePath; //Путь к файлу данных из аргументов
    bool do_log = false;
    std::string arg_str;
    bool exit_application = false;

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

            if (arg == "-f") { // Файл по умолчанию
                if (i + 1 >= argc) {
                    LOG_ERROR("После -f необходимо указать путь к файлу.");
                    continue;
                }

                config.filePath = argv[++i];
            }
            else if (arg == "-l") { // делать ли лог
                config.do_log = true;
            }
            else if (arg == "--help") {
                std::cout << HELP_COUT;
                input::getAnyKey();
                config.exit_application = true;
                return config;
            }
            else{
                LOG_WARN("Неизвестный тип аргумента");
                std::cout << "Неизвестный тип аргумента, используйте \"--help\" для справки";
                input::getAnyKey();
                config.exit_application = true;
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

    bool getExitApplication() const {
        return config.exit_application;
    }
};