#include "UI.h"
#include "Student.h"


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

StateType MenuLogic::menuShow()
{
    StateType next_state = StateType::NONE;
    while (next_state == StateType::NONE){
        beforeShow();
        std::cout << showUI();
        next_state = handleNav();
    }
    return next_state;
}

std::string MenuLogic::showUI() {

    size_t act_row = menu_out.act.row; //ссылка на активную строку для удобства
    static int first_row = 0;

    std::ostringstream ss;
    ss << menu_out.before_show; //Отображение шапки

    for (int i = 0; i < menu_out.total.row; i++) {
        if (menu_out.act.row == i) {
            ss << menu_out.ActMark;
        }
        else {
            ss << menu_out.InactMark;
        }
        if (menu_out.menu[i].show) {
            ss << menu_out.menu[i].name << ' '
                << menu_out.menu[i].param();
        }
    }
    ss << menu_out.post_show;


    return ss.str();
}

StateType MenuLogic::handleNav() {
    input::key key = input::getKey();
    MenuNav nav = GetAction(key);
    bool cont = true;
    switch (nav) {
    case MenuNav::Up:
        if (menu_out.act.row > 0) menu_out.act.row--;
        break;
    case MenuNav::Down:
        if (menu_out.act.row + 1 < menu_out.total.row) menu_out.act.row++;
        break;
    case MenuNav::Left:
        if (menu_out.act.col > 0) menu_out.act.col--;
        break;
    case MenuNav::Right:
        if (menu_out.act.col + 1 < menu_out.total.col) menu_out.act.col++;
        break;
    case MenuNav::Back:
        LOG_INFO("Пользователь нажал ESC. Выход из просмотрщика.");
        return menu_out.before_state;
    case MenuNav::Enter:
        LOG_INFO("Пользователь нажал Enter. Переход к редактированию.");
        return StateType::EditorMenu;
    default:
        break;
    }
    cont = menu_out.menu[menu_out.act.row].show;

    return StateType::NONE;
}

static size_t utf8Length(const std::string& str) //Узнаем длину строки в символах, а не в байтах, для корректного отображения русских букв
{
    size_t count = 0;

    for (unsigned char c : str)
    {
		if ((c & 0xC0) != 0x80) // Если старшие два бита не равны 10, значит это начало нового символа
            ++count;
    }

    return count;
}

static std::string padRight(const std::string& str, size_t width) // Функция для выравнивания строки по ширине, учитывая UTF-8 символы
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
static void addCell(
    std::ostringstream& ss,
    const std::string& value,
    size_t width,
    bool active)
{
    if (active)
        ss << "\033[30;47m";

    ss << " "
        << padRight(value, width)
        << " ";

    if (active)
        ss << "\033[0m";

    ss << "|";
}

Editor::Editor(StudentDB& students) : data(students) {
    LOG_INFO("Запуск конструктора Editor");
    menu_out.total.row = data.size(); // Устанавливаем общее количество строк в меню
    menu_out.total.col = static_cast<size_t>(data.INFO_COL_COUNT); // Общее количество колонок в меню
    menu_out.act = { 0, 0 }; // Начальная активная позиция

    size_t max_rows = menu_out.total.row;
    if (max_rows < 1) {
		LOG_WARN("В файле данных о студентах нет записи");
		max_rows = 1; // Чтобы не было деления на ноль и корректно отображалась шапка
        //data.addEmpty(); // Добавляем пустую запись, чтобы корректно отображалась шапка
	}
    

    menu_out.ActMark = "";
    menu_out.InactMark = "";
    menu_out.post_show = std::string(static_cast<size_t>(menu_width), '=') + '\n' + // Создаем строку из символов '=' длиной menu_width
        "Используйте стрелочки для навигации, ESC для выхода...\n";
};

void Editor::updateMenu()
{
    menu_out.total.row = data.size();

    size_t max_rows = data.size();
	if (max_rows == 0) {
		LOG_WARN("В файле данных о студентах нет записи");
		//data.addEmpty(); // Добавляем пустую запись, чтобы корректно отображалась шапка
        max_rows = 1;
	}

    num_width =
        static_cast<int>(std::to_string(max_rows).length());

    menu_width =
        num_width +
        data.NAME_WIDTH +
        data.GROUP_WIDTH +
        data.PASS_WIDTH +
        data.NUM_WIDTH +
        static_cast<int>(menu_out.total.col) +
        12;

    menu_out.before_show = header();

    menu_out.menu.resize(data.size());

    for (size_t i = 0; i < data.size(); ++i)
    {
        menu_out.menu[i].name = "";

        menu_out.menu[i].param =
            [this, i]() {
            return createString(i);
            };

        menu_out.menu[i].show = false;
        menu_out.menu[i].entered = true;

		menu_out.menu[i].state_aft_ent = StateType::EditorMenu;
    }
}

std::string Editor::createString(size_t row)
{
    std::ostringstream ss;

    bool active_row = (menu_out.act.row == row); //проверка, активна ли строка

    addCell(
        ss,
        std::to_string(data.getRow(row)),
        num_width,
        false
    );

    addCell(
        ss,
        data.getName(row),
        data.NAME_WIDTH,
        active_row && menu_out.act.col == 0
    );

    addCell(
        ss,
        data.getGroup(row),
        data.GROUP_WIDTH,
        active_row && menu_out.act.col == 1
    );

    addCell(
        ss,
        data.getPass(row),
        data.PASS_WIDTH,
        active_row && menu_out.act.col == 2
    );

    addCell(
        ss,
        data.getNum(row),
        data.NUM_WIDTH,
        active_row && menu_out.act.col == 3
    );

    ss << '\n';

    return ss.str();
}

std::string Editor::header()
{
    std::ostringstream ss;
    size_t max_rows = menu_out.total.row > 0 ? menu_out.total.row : 1;
    ss << " "; // Без этого шапка сьезжает на 1 символ влево
    addCell(
        ss,
        "#",
        num_width,
        false
    );
    addCell(
        ss,
        "Имя",
        data.NAME_WIDTH,
        false
    );
    addCell(
        ss,
        "Группа",
        data.GROUP_WIDTH,
        false
    );
    addCell(
        ss,
        "Пароль",
        data.PASS_WIDTH,
        false
    );
    addCell(
        ss,
        "Номер",
        data.NUM_WIDTH,
        false
    );
    ss << '\n';
    return ss.str();
}

bool Editor::beforeShow() {
    windowSize();

    if (data.empty()) { 
        LOG_ERROR("В файле данных о студентах нет записи"); 
        std::cout << "Нет данных для отображения.\n"; 
        return false; 
    }

    updateMenu();

    menu_out.total.row = menu_out.menu.size(); // Активная строка для удобства сокращенно
    
    static size_t first_row = 0; // изначально задаем 0

    //делаем прокрутку через отображение некотороых пунктов
	if (menu_out.act.row < MENU_NUM_TO_SHOW) { // Если вактивная стрелка в первых строках, то показываем с начала
        first_row = 0;
    }
	else if (menu_out.act.row >= first_row + MENU_SHOW_LENGTH - MENU_NUM_TO_SHOW) { // Если активная строка внизу, то прокручиваем вниз
        first_row = min(menu_out.act.row - MENU_SHOW_LENGTH + MENU_NUM_TO_SHOW, menu_out.total.row - MENU_SHOW_LENGTH);
    }
	else if (menu_out.act.row <= first_row + MENU_NUM_TO_SHOW) { 
        first_row = max(menu_out.act.row - MENU_NUM_TO_SHOW, 0);
    }

    size_t last_row = min(first_row + MENU_SHOW_LENGTH, menu_out.total.row); // Либо последняя строка, либо посчитали

    /*LOG_INFO(
        "Активный - " + std::to_string(menu_out.act.row) + 
        ", " + std::to_string(menu_out.act.col) +
        " Первый - " + std::to_string(first_row) +
        " Последний - " + std::to_string(last_row) +
        " Всего - " + std::to_string(menu_out.total.row)
    );*/

    for (size_t i = 0; i < menu_out.total.row; ++i) {
        menu_out.menu[i].show =
            i >= first_row && i < last_row; // выбираем показывать или нет строку
    }

    return true;
}



