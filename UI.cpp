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

    std::string header(int num_width, DataBase& data)
    {
        std::ostringstream ss;
        ss << " "; // Без этого шапка сьезжает на 1 символ влево
        const auto widths = data.getWidthVec();
        for(int i = 0; i < data.INFO_COL_COUNT; i++){
            addCell(
                ss,
                data.HEADER[i],
                widths[i]
            );
        }
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
    ss << menu_out.before_show << '\n'; //Отображение шапки

    for (size_t i = 0; i < menu_out.total.row; i++) {
        ss << (menu_out.act.row == i ? menu_out.ActMark : menu_out.InactMark);
        if (menu_out.menu[i].show) {
            ss << menu_out.menu[i].name;
            if (menu_out.menu[i].param)
                ss << ' ' << menu_out.menu[i].param() << '\n';
            else 
                ss << "\033[K\n";
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
    case MenuNav::Enter: {
        LOG_INFO("Пользователь нажал Enter.");
        system("cls"); //Иначе текст наславиается
        auto& item = menu_out.menu[menu_out.act.row];
        if (item.act_aft) item.act_aft();
        return item.state_aft;
    }
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

//=======================MainMenu======================

MainMenu::MainMenu(File& f) : file(f){
    LOG_INFO("Запуск конструктора MainMenu");
    menu_out.total.col = 1;
    menu_out.act = {0, 0};

    using S = StateType;
    menu_out.menu = {
        {.name = "Файл данных: "},
        {.name = "Меню просмотра данных", .state_aft = S::Viewer, .show = false},
        {.name = "Меню поиска", .state_aft = S::FindMenu},
        {.name = "Отменить изменения"},
        {.name = "Сохранить"},
        {.name = "Сохранить как"},
    };

    
}



//=======================Viewer=======================

Viewer::Viewer(DataBase& students, Position db_act) : data(students) {
    LOG_INFO("Запуск конструктора Viewer");
    LOG_INFO("Текущий фильтр: " + data.getFilterInf());
    menu_out.total.row = data.size(); // Устанавливаем общее количество строк в меню
    menu_out.total.col = static_cast<size_t>(data.INFO_COL_COUNT - 1); // Общее количество колонок в меню
    menu_out.act = db_act; // Начальная активная позиция

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

    if (data.sizeIfFilter() <= 0) {
        LOG_WARN("Нет полей соответсвующих фильтру");
        data.changeFilter(false);
    }

    menu_out.menu.resize(data.sizeIfFilter()); //делаем заренее размер массива

    if (menu_out.menu.size() <= 0) {
        LOG_ERROR("Размер базы = 0");
        return;
    }
    size_t _row = 0;
    for (size_t i = 0; i < data.size(); ++i)
    {
        if (data.isOnfilter() && !data.inFilter(i)) { // если данные фильтруются и поле не совпало
            continue;
        }
        
        menu_out.menu[_row].name = "";

        menu_out.menu[_row].param =
            [this, i]() {
            return createString(i);
            };

        menu_out.menu[_row].show = false;
        menu_out.menu[_row].enter = true;

		menu_out.menu[_row].state_aft = StateType::EditMenu;
        _row++;
    }

    menu_out.ActMark = "";
    menu_out.InactMark = "";
    menu_out.before_state = StateType::MainMenu;
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
    ss << "\033[K"; // Возвращаем форматирование

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

StateType Viewer::handleNav() {
    if (menu_out.tab_ent) {
        return StateType::ViewerTab;
    }
    else {
        return MenuLogic::handleNav();
    }
}

//=======================ViewerTab=======================
ViewerTab::ViewerTab(DataBase& db) : data(db) {
    LOG_INFO("Запуск конструктора ViewerTab");

    using S = StateType;
    menu_out.menu = {
        {.name = "Найти поле", .state_aft = S::FindMenu},
        {.name = "Найти и заменить поля", .state_aft = S::ReplaceAll},
        {.name = "Фильтр строк ", .param = [this]() {return data.getFilterInf(); } ,.state_aft = S::FilterMenu},
        {.name = "Отображение с фильтром", .param = [this]() {return filterShow(); }, .act_aft = [this]() {data.changeFilter(); }},
    };
    menu_out.before_show = "Выберите действие с просмотром данных";
    menu_out.post_show = "\n"
        "Используйте стрелки вверх вниз для навигации, Enter для выбора пункта, \n"
        "Нажмите Tab для возврата к просмотру данных, ESC для возврата в главное меню";
    menu_out.total.row = menu_out.menu.size();
    menu_out.before_state = S::Viewer;
}

std::string ViewerTab::filterShow() {
    if (data.isOnfilter())
        return "(Включено)";
    else
        return "(Выключено)";
}

StateType ViewerTab::handleNav() {
    if (menu_out.tab_ent) {
        return StateType::Viewer;
    }
    else {
        return MenuLogic::handleNav();
    }
}

//=======================EditMenu=======================

EditMenu::EditMenu(DataBase& base, Position& db_act) : data(base), pos(db_act){
    LOG_INFO("Запуск конструктора EditMenu");
    is_mod = false;

    using S = StateType;
    if (data.size() < 1) addStr(AddType::End); // Дабвляем хотя бы одну строку, если база совсем пустая
    menu_out.menu = {
        // Item::Row: строка студента, не выбирается
        {.param = [this]() { return createString(); }, .enter = false },

        {.name = "Изменить", .state_aft = S::EditField },   // делаем сейчас
        {.name = "Заменить все поля с данным именем", .state_aft = S::ReplaceAll},  // после основного меню: то же меню, только с введённым полем
        {.name = "Удалить строку", .act_aft = [this]() {deleteRow(); }, .state_aft = StateType::Viewer},  // легко, на уровне DataBase
        {.name = "Добавить строку сверху", .act_aft = [this]() {addStr(AddType::Before); }, .state_aft = StateType::Viewer },  // легко
        {.name = "Добавить строку снизу", .act_aft = [this]() {addStr(AddType::After); }, .state_aft = StateType::Viewer },  // легко
        {.name = "Вернуться к просмотру полей", .state_aft = S::Viewer },
    };

    //создаем образ меню
    menu_out.total.row = menu_out.menu.size();
    menu_out.total.col = data.INFO_COL_COUNT - 1;
    menu_out.act = { 1, pos.col }; // стартуем на «Изменить»
    menu_out.before_state = S::Viewer;

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
    if (data.size() < 1) addStr(AddType::End); // Дабвляем хотя бы одну строку, если база совсем пустая
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
		menu_out.act.row = -1; // Прикооол, оно unsigned, но подразумевается что это такое большое число, что оно точно не будет пунктом меню
    }
    else {
        pos.col = menu_out.act.col;
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
            system("cls"); //Иначе текст наслаивается
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

void EditMenu::deleteRow() { 
    data.removeRow(pos.row);
    if (pos.row >= data.size()) pos.row = data.size() - 1;
}

void EditMenu::addStr(AddType type) {
    switch (type) {
    case AddType::Before:
        data.insertRow(pos.row);
        break;
    case AddType::After:
        data.insertRow(pos.row + 1);
        ++pos.row;
        break;
    default:
        data.insertRow(data.size());
        pos.row = data.size() - 1;
        break;
    }
}

//=======================EditField=======================

EditField::EditField(DataBase& db, Position& p) : data(db), pos(p) {
    LOG_INFO("Запуск конструктора EditField");

    menu_out.menu = {
        {.param = [this]() { return createString(); }, .enter = false },
    };
    menu_out.total = { 1, 0 };
    menu_out.act = { 0, 0 };
    menu_out.ActMark = menu_out.InactMark = "";
    menu_out.before_state = StateType::EditMenu;

    menu_out.before_show = UI::header(data.numWidth(), data);
    menu_out.post_show = "\nВведите значение. Enter: сохранить, Esc: отмена\033[K";

    // max_len на 1 меньше ширины колонки, чтобы влез курсор ▌
    input_line.start(data.getField(pos.row, pos.col + 1),
        data.getWidthVec()[pos.col + 1]);
}

std::string EditField::createString() {
    std::ostringstream ss;
    for (size_t i = 0; i < static_cast<size_t>(data.INFO_COL_COUNT); i++) {
        bool cur = (i == pos.col + 1);
        UI::addCell(ss,
            cur ? input_line.display() : data.getField(pos.row, i),
            data.getWidthVec()[i],
            cur,
            UI::SelType::Selected);
    }
    ss << "\033[K\n";
    return ss.str();
}

StateType EditField::handleNav() {
    switch (input_line.handle(input::readKeyEvent())) {
    case input::TextInput::Result::Accepted:
        if (data.setField(pos.row, pos.col + 1, input_line.text())) {
            LOG_INFO("Поле изменено");
        }
        else {
             NotVerMenu ver_menu(input_line.text(), pos.col + 1);
             ver_menu.menuShow();
        }
        return StateType::EditMenu;
    case input::TextInput::Result::Cancelled:
        return StateType::EditMenu;
    default:
        return StateType::NONE;
    }
}

//======================FindMenuLogic============================

FindMenuLogic::FindMenuLogic(DataBase& d, Position& p, StateType before_state) : data(d), pos(p), field_int(p.col + 1) {
    field = data.HEADER[field_int];
    find_str = data.getField(pos.row, field_int);
    repl_str = find_str;
    
    using S = StateType;
    menu_out.menu = {
        {.name = "Поле: ", .param = [this]() {return line(0, field); }, .act_aft = [this]() {changeField(); }},
        {.name = "Найти: ", .param = [this]() {return line(1, find_str); }, .act_aft = [this]() { startInput(); }},
        {.name = "Заменить на: ", .param = [this]() {return line(2, repl_str); }, .act_aft = [this]() { startInput(); }},
        {.name = "Найдено совпадений: ", .param = [this]() {return std::to_string(match()); }, .enter = false },
        {.name = "Заменить ", .act_aft = [this]() {editAll(); }} // Добавить функцию замены
    };
    menu_out.total = { menu_out.menu.size(), static_cast<size_t>(data.INFO_COL_COUNT - 1) };
    menu_out.before_state = before_state;
}

size_t FindMenuLogic::match() const {
    std::string rt_find = (typing && menu_out.act.row == 1) ? input_line.text() : find_str;
    return data.getFilterHit(field_int, rt_find).size(); 
}

std::string FindMenuLogic::line(size_t item, const std::string& value) const {
    bool editing = typing && menu_out.act.row == item;
    return (editing ? input_line.display() : value) + "\033[K";
}

void FindMenuLogic::startInput() {
    if (typing) {
        LOG_WARN("Вызов функции начала ввода, хотя ввод уже производится");
        return;
    }
    size_t field_pos = menu_out.act.row;

    if (field_pos == 1) {
        target = &find_str;
    }
    else if (field_pos == 2) {
        target = &repl_str;
    }
    else {
        return;
    }
    input_line.start(
        *target, 
        data.getWidthVec()[field_int]
    );
    typing = true;
    system("cls"); //Возможно наложение текста
}

void FindMenuLogic::changeField() {
    if (pos.col + 1 < data.INFO_COL_COUNT - 1) {
        ++pos.col;
    }
    else {
        pos.col = 0;
    }

    field_int = pos.col + 1;
    field = data.HEADER[field_int];
    system("cls"); //Происходит наложение текста
}

void FindMenuLogic::editAll(){
    if (!data.editAllFind(field_int, find_str, repl_str)) {
        NotVerMenu ver_menu(input_line.text(), pos.col + 1);
        ver_menu.menuShow();
    }
    else
        LOG_INFO("Поля изменены");
}

bool FindMenuLogic::beforeShow() {
    system("cls");
    if (typing)
        menu_out.ActMark = "\033[0m";
    else 
        menu_out.ActMark = "\033[30;47m";
    return MenuLogic::beforeShow();
}

StateType FindMenuLogic::handleNav() {
    if (!typing) return MenuLogic::handleNav();      // обычная навигация по пунктам

    switch (input_line.handle(input::readKeyEvent())) {
    case input::TextInput::Result::Accepted:
        *target = input_line.text();
        typing = false;
        target = nullptr;
        break;
    case input::TextInput::Result::Cancelled:        // Esc отменяет только ввод, из меню не выходит
        typing = false;
        target = nullptr;
        break;
    default:
        break;
    }
    return StateType::NONE;
}

//==========================ReplaceMenu================================
ReplaceMenu::ReplaceMenu(DataBase& d, Position& p, StateType b_s) : FindMenuLogic(d, p, b_s){
    menu_out.act.row = 2;
}

//==========================FilterMenu================================
FilterMenu::FilterMenu(DataBase& d, Position& p, StateType b_s) : FindMenuLogic(d, p, b_s) {
    field_int = data.getFilterCol();
    find_str = data.getFilterStr();
    menu_out.menu[1].name = "Фильтр по тексту";
    menu_out.menu[2].show = false;
    menu_out.menu[4].name = "Применить фильтр";
    menu_out.menu[4].act_aft = [this]() {return data.changeFilter(field_int, find_str, true); };
    menu_out.menu[4].state_aft = StateType::ViewerTab;
    menu_out.before_state = StateType::ViewerTab;
}

//==========================FindMenu================================
FindMenu::FindMenu(DataBase& d, Position& p, StateType b_s) : FindMenuLogic(d, p, b_s) {
    pos_of_find = { p.row, p.col };
    menu_out.menu[2].show = false;
    menu_out.menu[4].name = "";
    menu_out.menu[4].param = [this]() { return createString(); };
    menu_out.menu[4].act_aft = [this]() { pos = pos_of_find; };   // перейти к найденной строке
    menu_out.menu[4].state_aft = StateType::Viewer;
    menu_out.before_state = StateType::Viewer;
}

bool FindMenu::beforeShow() {
    bool res = FindMenuLogic::beforeShow();
    std::string rt_find = (typing && menu_out.act.row == 1) ? input_line.text() : find_str;

    std::vector<size_t> hits = data.getFilterHit(field_int, rt_find);
    menu_out.total.col = hits.size();

    if (hits.empty()) {
        menu_out.menu[4].show = false;
        menu_out.act.col = 0;
        pos_of_find.row = pos.row;            // оставляем прежнюю позицию
    }
    else {
        menu_out.menu[4].show = true;
        if (menu_out.act.col >= hits.size())
            menu_out.act.col = hits.size() - 1;
        pos_of_find.row = hits[menu_out.act.col];
    }
    pos_of_find.col = field_int - 1;          // без колонки "#"

    return res;
}

Position FindMenu::getPos() {
    return pos_of_find;
}

std::string FindMenu::createString() {
    std::ostringstream ss;
    for (size_t i = 0; i < static_cast<size_t>(data.INFO_COL_COUNT); i++) {
        UI::addCell(
            ss,
            data.getField(pos_of_find.row, i),
            data.getWidthVec()[i]
        );
    }
    ss << "\033[K\n";
    return ss.str();
}



