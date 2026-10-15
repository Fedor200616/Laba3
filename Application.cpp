#include "Application.h"
#include <iostream>

Application::Application(int argc, char* argv[])
    : app_info(argc, argv),
    file_manager(app_info.getStartPath()) {
    Log::getInstance().init(
        app_info.getExeDirectory(),
        app_info.getConfig().do_log
    );
	LOG_INFO("Запуск конструктора Application");
	std::string log_message = "Приложение запущено с аргументами: " + pathToUtf8(app_info.getConfig().arg_str);

    LOG_INFO(log_message);

    fs::path start_path = app_info.getStartPath();
    if (!start_path.empty() && fs::exists(start_path)) {
        auto loaded_students = file_manager.loadFromFile();
        students_info.load(loaded_students);

        current_state = StateType::Viewer;
        start_with_path = true;
    }
    else {
        LOG_WARN("Файл не указан или не найден.");
    }
    db_act_pos = { 0, 0 };
}

void Application::run() {
    // Первоначальная очистка экрана
    std::cout << "\033[2J\033[H";
	
    while (current_state != StateType::EXIT) {
		system("cls"); // Очистка экрана для Windows
        switch (current_state) {
        case StateType::Viewer:{ 
            Viewer Viewer(students_info, db_act_pos);
            current_state = Viewer.menuShow();
            db_act_pos = Viewer.getActivePosition();
            break;
        }
        case StateType::EditMenu: {
            //LOG_WARN("Данный функционал еще не реализован. Возврат в меню редактирования.");
            LOG_INFO("Выбрано поле для редактирования: строка " + std::to_string(db_act_pos.row) + ", колонка " + std::to_string(db_act_pos.col) + ", название " + students_info.getField(db_act_pos.row, db_act_pos.col + 1));
            EditMenu edit_menu(students_info, db_act_pos);
            current_state = edit_menu.menuShow();
            break;
        }
        case StateType::EditField: {
            LOG_INFO("изменение поля: строка " + std::to_string(db_act_pos.row) + ", колонка " + std::to_string(db_act_pos.col) + ", название " + students_info.getField(db_act_pos.row, db_act_pos.col + 1));
            EditField edit_field(students_info, db_act_pos);
            current_state = edit_field.menuShow();
            break;
        }
        case StateType::EditAllField: {
            LOG_INFO("Изменение или поиск всех полей");
            EditAllField edit_all(students_info, db_act_pos);
            current_state = edit_all.menuShow();
            break;
        }
        case StateType::_ERROR:
			LOG_ERROR("Произошла ошибка. Завершение работы программы.");
			std::cout << "Произошла ошибка. Завершение работы программы.\n";
			std::cout << "Нажмите любую клавишу для выхода...";
			input::getAnyKey();
			current_state = StateType::EXIT;
			break;
        default:
			LOG_ERROR("Неизвестное состояние: " + std::to_string(static_cast<int>(current_state)));
            std::cout << "Запустите программу с флагом -f <путь_к_файлу>\n";
            std::cout << "Пример: ./program -f students.txt -l\n\n";
            std::cout << "Нажмите любую клавишу для выхода...";
            input::getAnyKey();
			current_state = StateType::EXIT;
            break;
        }
    }
}