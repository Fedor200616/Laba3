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

    bool start_with_path = false;

	Position db_act_pos; // Позиция активного элемента в данных

public:
    Application(int argc, char* argv[]);

    void run();

};

