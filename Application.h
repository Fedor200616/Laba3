#pragma once
#include <string>
#include <filesystem>
#include <vector>

#include "States.h"
#include "File.h"
#include "UI.h"
#include "Main.h"
#include "Log.h"

/// <summary>
/// три задачи: запуск приложения, координацию компонентов и переключение состояний. Конкретную работу делегировать соответствующим классам.
/// </summary>
class Application {
private:
    StateType current_state = StateType::MainMenu;

    Info app_info;
    File file_manager;
    DataBase students_info;
    Explorer explorer;

    bool start_with_path = false;

	Position db_act_pos; // Позиция активного элемента в данных

    DirectoryEntry selEntry; //Файл, который выбралы для Таб меню

public:
    Application(int argc, char* argv[]);

    void run();
};

class BackUpHandle {
private:
    DataBase& base;
    File& file;
    Info& programm_info;

    std::vector<StudentInfo> ref_info; //Копия массива с первого открытия файла

    void instantBackup();
    void check_mod();
    
public:
    void updateRef();
    void backupUpdate();

};
