#include "Application.h"
#include <iostream>

Application::Application(int argc, char* argv[])
    : app_info(argc, argv),
    file_manager(app_info.getStartPath()),
    ui(app_info, current_state)
{
    Log::getInstance().init(
        app_info.getExeDirectory(),
        app_info.getConfig().do_log
    );
    LOG_INFO("Приложение инициализировано");

    fs::path start_path = app_info.getStartPath();
    if (!start_path.empty() && fs::exists(start_path)) {
        auto loaded_students = file_manager.loadFromFile();
        students_info.load(loaded_students);

        current_state = StateType::EditorView;
        start_with_path = true;
    }
    else {
        LOG_WARN("Файл не указан или не найден.");
    }
}

void Application::run() {
    // Первоначальная очистка экрана
    std::cout << "\033[2J\033[H";

    while (current_state != StateType::EXIT) {
        if (current_state == StateType::EditorView) {
            Editor editor(ui, students_info);
            current_state = editor.menuShow();
        }
        else {
            std::cout << "Запустите программу с флагом -f <путь_к_файлу>\n";
            std::cout << "Пример: ./program -f students.txt -l\n\n";
            std::cout << "Нажмите любую клавишу для выхода...";
            input::getAnyKey();
            break;
        }
    }
}