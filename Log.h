#pragma once
#include <fstream>
#include <filesystem>
#include <string>
#include <chrono>
#include <format> 

namespace fs = std::filesystem;

//Синглтон, обьект один

class Log {
private:
    fs::path log_path;
    bool need_log = false;

    Log() = default; // Закрытый конструктор, гарантия единственного обьекта

public:
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

    static Log& getInstance() {
        static Log instance;
        return instance;
    }

    void init(fs::path exe_directory, bool enable_logging) {
        log_path = exe_directory / "app.log";
        need_log = enable_logging;
    }

    void write(const std::string& level, const std::string& message) {
        if (!need_log) return;

        std::ofstream log_file(log_path, std::ios::app);
        if (log_file.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto local = std::chrono::current_zone()->to_local(now);
            
            log_file << std::format("[{:%Y-%m-%d %H:%M:%S}] [{}] {}\n", 
                        std::chrono::floor<std::chrono::seconds>(local), 
                        level, 
                        message);
        }
    }
};

// --- УДОБНЫЕ МАКРОСЫ ДЛЯ ВЫЗОВА ИЗ ЛЮБОГО КЛАССА ---
#define LOG_INFO(msg)  Log::getInstance().write("INFO ", msg)
#define LOG_WARN(msg)  Log::getInstance().write("WARN ", msg)
#define LOG_ERROR(msg) Log::getInstance().write("ERROR", msg)
