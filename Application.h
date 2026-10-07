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
    StudentDB students_info;

    bool start_with_path = false;

	Position act_pos; // Позиция активного элемента в меню Editor

public:
    Application(int argc, char* argv[]);

    void run();

};

