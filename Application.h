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
/// три задачи: запуск приложени€, координацию компонентов и переключение состо€ний.  онкретную работу делегировать соответствующим классам.
/// </summary>
class Application {
private:
    StateType current_state = StateType::MainMenu;

    Info app_info;
    File file_manager;
    StudentDB students_info;

    bool start_with_path = false;

    UI_Interface ui;

public:
    Application(int argc, char* argv[]);

    void run();

};

/*ѕлавность: „тобы консоль не мерцала при посто€нной перерисовке в фазе Show UI, очищайте экран не через system("cls"), а перемеща€ каретку в начало (0,0) через ANSI-последовательность \033[H или SetConsoleCursorPosition.*/