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

    UI_Interface ui;

public:
    Application(int argc, char* argv[]);

    void run();

};

/*Плавность: Чтобы консоль не мерцала при постоянной перерисовке в фазе Show UI, очищайте экран не через system("cls"), а перемещая каретку в начало (0,0) через ANSI-последовательность \033[H или SetConsoleCursorPosition.*/