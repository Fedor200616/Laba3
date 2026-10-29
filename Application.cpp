#include "Application.h"
#include <iostream>

//Конструктор главного управленца
Application::Application(int argc, char* argv[])
    : app_info(argc, argv),
    file_manager(app_info.getStartPath()),
    explorer(app_info.getStartPath()){
    Log::getInstance().init(
        app_info.getExeDirectory(),
        app_info.getConfig().do_log
    );
	LOG_INFO("Запуск конструктора Application");
	std::string log_message = "Приложение запущено с аргументами: " + pathToUtf8(app_info.getConfig().arg_str); 

    LOG_INFO(log_message);
    
    fs::path start_path = app_info.getStartPath();
    if (!start_path.empty() && fs::exists(start_path)) { // Импортируем файл, если он указан в флагах запуска
        auto loaded_students = file_manager.loadFromFile(); 
        students_info.load(loaded_students);

        current_state = StateType::Viewer;
        start_with_path = true;
    }
    else {
    }
    db_act_pos = { 0, 0 };
}

// Основная функция обработки состояний
void Application::run() {
    // Первоначальная очистка экрана
    std::cout << "\033[2J\033[H";
	StateType prev_state = StateType::MainMenu;
    while (current_state != StateType::EXIT) {
		system("cls"); // Очистка экрана для Windows
        StateType buf_state = current_state;
        switch (current_state) {
        case StateType::MainMenu:{
            MainMenu menu(file_manager);
            current_state = menu.menuShow();
            break;
        }
        case StateType::OpenExplorer: {
            static fs::path path_buf = file_manager.getFilePath();
            if (path_buf.empty()) path_buf = app_info.getExe();
            LOG_INFO("Путь для ExplorerMenuLogic " + pathToUtf8(path_buf));
            ExplorerMenuLogic menu(path_buf);
            current_state = menu.menuShow();
            if (current_state == StateType::ExplorerOutFile) {
                bool norm = true;
                File buf(menu.getSelFile());
                if (buf.exist() && buf.checkHeader()) {
                    if (file_manager.setFile(menu.getSelFile())) {
                        auto loaded_students = file_manager.loadFromFile();
                        students_info.load(loaded_students);

                        db_act_pos = { 0, 0 };
                        current_state = StateType::MainMenu;
                        path_buf = buf.getFilePath();
                    }
                }
                else {
                    LOG_ERROR("Выбранный файл не существует или имеет неверную шапку");
                    std::cout << "\nНеверный формат файла! Выберите другой.\n";
                    input::getAnyKey();

                    current_state = buf_state;
                }
            }
            break;
        }
        case StateType::Viewer:{ // Меню просмотра полей студентов
            Viewer viewer(students_info, db_act_pos);
            current_state = viewer.menuShow();
            db_act_pos = viewer.getActivePosition();
            break;
        }
        case StateType::ViewerTab: {
            ViewerTab viewer_tab(students_info);
            current_state = viewer_tab.menuShow();
            break;
        }
        case StateType::EditMenu: { // Меню работы с полем
            //LOG_WARN("Данный функционал еще не реализован. Возврат в меню редактирования.");
            LOG_INFO("Выбрано поле для редактирования: строка " + std::to_string(db_act_pos.row) + ", колонка " + std::to_string(db_act_pos.col) + ", название " + students_info.getField(db_act_pos.row, db_act_pos.col + 1));
            EditMenu edit_menu(students_info, db_act_pos);
            current_state = edit_menu.menuShow();
            break;
        }
        case StateType::EditField: { // меню ввода строки в поле
            LOG_INFO("изменение поля: строка " + std::to_string(db_act_pos.row) + ", колонка " + std::to_string(db_act_pos.col) + ", название " + students_info.getField(db_act_pos.row, db_act_pos.col + 1));
            EditField edit_field(students_info, db_act_pos);
            current_state = edit_field.menuShow();
            break;
        }
        case StateType::ReplaceAll: { // класс замены полей выделенной колнки
            LOG_INFO("Изменение или поиск всех полей");
            ReplaceMenu menu(students_info, db_act_pos, StateType::Viewer);
            current_state = menu.menuShow();
            break;
        }
        case StateType::FilterMenu: {
            LOG_INFO("Меню фильтра");
            FilterMenu menu(students_info, db_act_pos, StateType::Viewer);
            current_state = menu.menuShow();
            break;
        }
        case StateType::FindMenu: {
            LOG_INFO("Меню поиска");
            FindMenu menu(students_info, db_act_pos, StateType::Viewer);
            current_state = menu.menuShow();
            db_act_pos = menu.getPos();
            break;
        }
        case StateType::_ERROR:
			LOG_ERROR("Произошла ошибка. Завершение работы программы.");
			std::cout << "Произошла ошибка. Завершение работы программы.\n";
			std::cout << "Нажмите любую клавишу для возврата в главное меню...\n";
			input::getAnyKey();
			current_state = StateType::MainMenu;
			break;
        default:
			LOG_ERROR("Неизвестное состояние: " + std::to_string(static_cast<int>(current_state)));
            std::cout << "Неизвестная ошибка\n";
            std::cout << "Нажмите любую клавишу для возврата в главное меню...\n";
            input::getAnyKey();
			current_state = StateType::MainMenu;
            break;
        }
        prev_state = buf_state;
    }
}   

