#include "UI.h"
#include "Student.h"


namespace UI {
    size_t utf8Length(const std::string& str) //Узнаем длину строки в символах, а не в байтах, для корректного отображения русских букв
    {
        size_t count = 0;

        for (unsigned char c : str)
        {
            if ((c & 0xC0) != 0x80) // Если старшие два бита не равны 10, значит это начало нового символа
                ++count;
        }

        return count;
    }

    std::string padRight(const std::string& str, size_t width) // Функция для выравнивания строки по ширине, учитывая UTF-8 символы
    {
        size_t length = utf8Length(str);

        if (length >= width)
            return str;

        return str + std::string(width - length, ' ');
    }

    /// <summary>
    /// Вывод на экран отдельной ячейки
    /// </summary>
    /// <param name="ss">поток ввода</param>
    /// <param name="value">текст</param>
    /// <param name="width">Ширина ячейки</param>
    /// <param name="active">Активна ли ячейка</param>
    void addCell(
        std::ostringstream& ss,
        const std::string& value,
        size_t width,
        bool active,
        SelType cell_type
    )
    {
        std::string cell_str;
        switch (cell_type) {
        case SelType::Selected:
            cell_str = "\033[30;47m";          // серый фон
            break;

        case SelType::Yellow:
            cell_str = "\033[30;43m";          // жёлтый фон
            break;
        case SelType::Underlined:
            cell_str = "\033[4;97;40m";
            break;
        default:
            break;
        }
        if (active)
            ss << cell_str;

        ss << " "
            << padRight(value, width)
            << " ";

        if (active)
            ss << "\033[0m";

        ss << "|";
    }

    std::string header(int num_width, StudentDB& data)
    {
        std::ostringstream ss;
        ss << " "; // Без этого шапка сьезжает на 1 символ влево
        for(int i = 0; i < data.INFO_COL_COUNT; i++){
            addCell(
                ss,
                data.HEADER[i],
                data.getWidthVec()[i],
                false
            );
        }
        ss << '\n';
        return ss.str();
    }
}

MenuNav MenuLogic::GetAction(input::key key_code) const
{
    switch (key_code)
    {
    case input::key::Up:
        return MenuNav::Up;

    case input::key::Down:
        return MenuNav::Down;

    case input::key::Left:
        return MenuNav::Left;

    case input::key::Right:
        return MenuNav::Right;

    case input::key::Enter:
        return MenuNav::Enter;

    case input::key::Esc:
        return MenuNav::Back;

    case input::key::Tab:
        return MenuNav::KeyTab;

    default:
        return MenuNav::Unexpected; // возвращает для других значений
    }
}

bool MenuLogic::beforeShow() {
    windowSize();
    return true;
}

StateType MenuLogic::menuShow()
{
    StateType next_state = StateType::NONE;
    while (next_state == StateType::NONE){
		if (!beforeShow()) {
			LOG_ERROR("Ошибка перед показом меню. Выход из меню.");
			return StateType::_ERROR;
		}
        std::cout << showUI() << "\033[J";   // стереть всё ниже курсора
        next_state = handleNav();
    }
    return next_state;
}

std::string MenuLogic::showUI() {

    size_t act_row = menu_out.act.row; //ссылка на активную строку для удобства
    static int first_row = 0;

    std::ostringstream ss;
	ss << "\033[0m"; // Сброс цвета и стиля
    ss << menu_out.before_show; //Отображение шапки

    for (size_t i = 0; i < menu_out.total.row; i++) {
        ss << (menu_out.act.row == i ? menu_out.ActMark : menu_out.InactMark);
        if (menu_out.menu[i].show) {
            ss << menu_out.menu[i].name;
            if (menu_out.menu[i].param) ss << ' ' << menu_out.menu[i].param();
            else ss << "\033[K\n";
        }
    }
    ss << "\033[0m";
    ss << menu_out.post_show;


    return ss.str();
}


StateType MenuLogic::handleNav() {
    input::key key = input::getKey();
    MenuNav nav = GetAction(key);
	size_t act_row = menu_out.act.row; //буфер активной строки
    int itter = 0;
    switch (nav) {
    case MenuNav::Up:
        do {
            if (menu_out.act.row > 0) {
                menu_out.act.row--;
            }
            itter++;
			if (itter > menu_out.total.row) {
				LOG_ERROR("Ошибка в навигации по меню.");
                menu_out.act.row = act_row;
			}
        } while (
            !menu_out.menu[menu_out.act.row].show ||
            !menu_out.menu[menu_out.act.row].enter
            );
        break;
    case MenuNav::Down:
        do {
            if (menu_out.act.row + 1 < menu_out.total.row) {
                menu_out.act.row++;
            }
            itter++;
            if (itter > menu_out.total.row) {
                LOG_ERROR("Ошибка в навигации по меню.");
                menu_out.act.row = act_row;
            }
        } while (
            !menu_out.menu[menu_out.act.row].show ||
            !menu_out.menu[menu_out.act.row].enter
            );
        break;
    case MenuNav::Left:
        if (menu_out.act.col > 0) menu_out.act.col--;
        break;
    case MenuNav::Right:
        if (menu_out.act.col + 1 < menu_out.total.col) menu_out.act.col++;
        break;
    case MenuNav::Back:
        LOG_INFO("Пользователь нажал ESC.");
        return menu_out.before_state;
    case MenuNav::Enter:
        LOG_INFO("Пользователь нажал Enter. Переход к редактированию.");
        return menu_out.menu[menu_out.act.row].state_aft_ent;
    case MenuNav::KeyTab:
        LOG_INFO("Нажатие Таба");
        system("cls"); //Иначе текст наславиается
        menu_out.tab_ent = !menu_out.tab_ent;
        break;
    default:
        break;
    }

    return StateType::NONE;
}



Viewer::Viewer(StudentDB& students) : data(students) {
    LOG_INFO("Запуск конструктора Viewer");
    menu_out.total.row = data.size(); // Устанавливаем общее количество строк в меню
    menu_out.total.col = static_cast<size_t>(data.INFO_COL_COUNT); // Общее количество колонок в меню
    menu_out.act = { 0, 0 }; // Начальная активная позиция

    size_t max_rows = menu_out.total.row;
    if (max_rows < 1) {
		LOG_WARN("В файле данных о студентах нет записи");
		max_rows = 1; // Чтобы не было деления на ноль и корректно отображалась шапка
        //data.addEmpty(); // Добавляем пустую запись, чтобы корректно отображалась шапка
	}

    menu_width =
        data.numWidth() +
        data.getWidthVec()[0] +
        data.getWidthVec()[1] +
        data.getWidthVec()[2] +
        data.getWidthVec()[3] +
        static_cast<int>(menu_out.total.col) +
        12; // нужно для ровного отображения получено эксп. путем

    menu_out.before_show = UI::header(data.numWidth(), data);
    menu_out.post_show = std::string(static_cast<size_t>(menu_width), '=') + '\n' + // Создаем строку из символов '=' длиной menu_width
        "Используйте стрелочки для навигации, ESC для выхода...\n";

    menu_out.menu.resize(data.size());

    for (size_t i = 0; i < data.size(); ++i)
    {
        menu_out.menu[i].name = "";

        menu_out.menu[i].param =
            [this, i]() {
            return createString(i);
            };

        menu_out.menu[i].show = false;
        menu_out.menu[i].enter = true;

		menu_out.menu[i].state_aft_ent = StateType::ViewerMenu;
    }

    menu_out.ActMark = "";
    menu_out.InactMark = "";
    
};

std::string Viewer::createString(size_t row)
{
    std::ostringstream ss;

    bool active_row = (menu_out.act.row == row); //проверка, активна ли строка

    for(int i = 0; i < data.INFO_COL_COUNT; i++){
        bool act = active_row && (i == menu_out.act.col + 1); //-1 т.к. нельзя выбирать номер строки
        UI::addCell(
            ss,
            data.getField(row, i),
            data.getWidthVec()[i],
            act
        );
    }
    ss << "\033[K\n";   // вместо ss << '\n'

    return ss.str();
}

bool Viewer::beforeShow() {
    windowSize();

    if (data.empty()) { 
        LOG_ERROR("В файле данных о студентах нет записи"); 
        std::cout << "Нет данных для отображения.\n"; 
        return false; 
    }    

    menu_out.total.row = menu_out.menu.size(); // Активная строка для удобства сокращенно
    
    static size_t first_row = 0; // изначально задаем 0

    //делаем прокрутку через отображение некотороых пунктов
	if (menu_out.act.row < MENU_NUM_TO_SHOW) { // Если активная стрелка в первых строках, то показываем с начала
        first_row = 0;
    }
	else if (menu_out.act.row >= first_row + MENU_SHOW_LENGTH - MENU_NUM_TO_SHOW) { // Если активная строка внизу, то прокручиваем вниз
        first_row = min(menu_out.act.row - MENU_SHOW_LENGTH + MENU_NUM_TO_SHOW, menu_out.total.row - MENU_SHOW_LENGTH);
    }
	else if (menu_out.act.row <= first_row + MENU_NUM_TO_SHOW) { 
        first_row = max(menu_out.act.row - MENU_NUM_TO_SHOW, 0);
    }

    size_t last_row = min(first_row + MENU_SHOW_LENGTH, menu_out.total.row); // Либо последняя строка, либо посчитали

    for (size_t i = 0; i < menu_out.total.row; ++i) {
        menu_out.menu[i].show =
            i >= first_row && i < last_row; // выбираем показывать или нет строку
    }
    return true;
}

EditMenu::EditMenu(StudentDB& base, Position act) : data(base), pos(act){
    LOG_INFO("Запуск конструктора EditMenu");
    temp_info = data.getInfoVec();
    is_mod = false;
    data.getWidthVec()[0] = static_cast<int>(std::to_string(data.size()).length());
    //создаем образ меню
    menu_out.total.row = menu_str.size();
    menu_out.total.col = data.INFO_COL_COUNT;
    menu_out.act = { 1, pos.col };
    menu_out.before_state = StateType::Viewer;
    menu_out.menu.resize(menu_str.size());
    for (size_t i = 1; i < menu_str.size(); ++i) {
        menu_out.menu[i].name = menu_str[i];
        menu_out.menu[i].show = true;
        menu_out.menu[i].enter = true;
        menu_out.menu[i].state_aft_ent = StateType::ViewerMenu; // После выбора пункта меню остаемся в этом же меню
    }
	menu_out.menu[0].param = [this]() { return createString(); };
	menu_out.menu[0].enter = false; // Первый пункт меню не выбирается, он просто отображает текущую строку

    menu_out.before_show = UI::header(data.getWidthVec()[0], data);
    menu_out.post_show = "\n"
        "Используйте стрелки вверх вниз для навигации, Enter для выбора пункта, \n"
        "Нажмите Tab для изменения выбора поля редактирования, Esc для возврата в меню просмотра";
}

std::string EditMenu::createString()
{
    std::ostringstream ss;
    UI::SelType sel_type = menu_out.tab_ent ? UI::SelType::Selected : UI::SelType::Underlined;

    for (int i = 0; i < data.INFO_COL_COUNT; i++) {
        UI::addCell(
            ss,
            data.getField(pos.row, i),
            data.getWidthVec()[i],
            pos.col == i - 1,
            sel_type
        );
    }

    ss << "\033[K\n";   // вместо ss << '\n'

    return ss.str();
}

bool EditMenu::beforeShow() {
	bool res = MenuLogic::beforeShow();
	if (data.empty()) {
		LOG_ERROR("В файле данных о студентах нет записи");
		std::cout << "Нет данных для отображения.\n";
		return 0;
	}
	menu_out.total.row = menu_out.menu.size(); // Активная строка для удобства сокращенно
    if (menu_out.tab_ent) {
        menu_out.post_show = "\n"
            "Выберите поле в таблице.\n"
            "Используйте стрелки вверх вниз для навигации, Enter для выбора пункта, \n"
            "Нажмите ESC для возврата в меню просмотра";
		menu_out.act.row = -1; // Если мы редактируем поле, то активная строка - это строка с полем
    }
    else {
        menu_out.post_show = "\n"
            "Используйте стрелки вверх вниз для навигации, Enter для выбора пункта, \n"
            "Нажмите Tab для изменения выбора поля редактирования, Esc для возврата в меню просмотра";
    }
	return res;
}

StateType EditMenu::handleNav() {
	if (!menu_out.tab_ent) { // Меню редактирования поля
        return MenuLogic::handleNav();
    }
    else { // меню перемещения между полями
        LOG_INFO("Меню перемещения между полями");
        input::key key = input::getKey();
        MenuNav nav = GetAction(key);
        size_t act_row = menu_out.act.row; //буфер активной строки
        int itter = 0;
        switch (nav) {
        case MenuNav::KeyTab:
        case MenuNav::Enter:
            LOG_INFO("Нажатие Таба");
            system("cls"); //Иначе текст наславиается
            menu_out.tab_ent = !menu_out.tab_ent;
			menu_out.act.row = 1; // Возвращаем активную строку на первую строку меню, 0 - это строка с полем
            break;
        case MenuNav::Up:
            if (pos.row > 0) pos.row--; //Поднимаем указатель строки
            break;
        case MenuNav::Down:
            if (pos.row + 1 < data.size()) pos.row++; //Опускаем указатель строки
            break;
        case MenuNav::Left:
            if (pos.col > 0) pos.col--;
            break;
        case MenuNav::Right:
            if (pos.col + 1 < data.INFO_COL_COUNT - 1) pos.col++; //-1 т.к. нельзя выбирать номер строки
            break;
        case MenuNav::Back:
            LOG_INFO("Пользователь нажал ESC.");
            return menu_out.before_state;
        default:
            break;
        }
    }


	return StateType::NONE;
}
