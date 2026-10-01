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
        showUI();
        next_state = handleNav();
    }
    return next_state;
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

Editor::Editor(UI_Interface& ui_inter, StudentDB& students) : ui(ui_inter), data(students) {
    menu_out.total.row = data.size(); // Устанавливаем общее количество строк в меню
    menu_out.total.col = static_cast<size_t>(data.INFO_COL_COUNT); // Общее количество колонок в меню
    menu_out.act = { 0, 0 }; // Начальная активная позиция

    size_t max_rows = menu_out.total.row > 0 ? menu_out.total.row : 1;
    num_width = static_cast<int>(std::to_string(max_rows).length());
    menu_width = num_width + data.NAME_WIDTH + data.GROUP_WIDTH + data.PASS_WIDTH + data.NUM_WIDTH + static_cast<int>(menu_out.total.col);

    menu_out.before_show = header();

    // Выделяем память / меняем размер вектора под размер данных
    menu_out.menu.resize(data.size());
    for (size_t i = 0; i < data.size(); i++)
    {
        menu_out.menu[i].name = "";
        menu_out.menu[i].param = [this, i]() { return createString(i); }; // Используем в параметре для изменения, this - указатель на текущий объект класса Editor, i - индекс строки
        menu_out.menu[i].show = false; //Показ самих пунктов будет определять функция showUI, в зависимости от того, попадает ли пункт в видимую область
        menu_out.menu[i].entered = true; //Пусть всегда будет как заполнен, мб сделать пустые строки как false, ониж типо не заполнены?
    }
    menu_out.ActMark = "";
    menu_out.InactMark = "";
    menu_out.post_show = std::string(static_cast<size_t>(menu_width), '=') + '\n' + // Создаем строку из символов '=' длиной menu_width
        "Используйте стрелочки для навигации, ESC для выхода...\n";
};

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
    int header_num_width = static_cast<int>(std::to_string(max_rows).length());
    addCell(
        ss,
        "#",
        header_num_width,
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
    ui.windowSize();
    return true;
}

bool Editor::showUI() {
    std::cout << menu_out.before_show;

    if (data.empty()) {
        LOG_ERROR("Файл пуст или не удалось прочитать данные");
        std::cout << "  [Файл пуст или не удалось прочитать данные]\n";
    }
    else {
        for (size_t i = 0; i < data.size(); ++i) {
            std::cout << createString(i);
        }
    }

    std::cout << menu_out.post_show;
    return true;
}

StateType Editor::handleNav() {
    input::key key = input::getKey();
    MenuNav nav = GetAction(key);

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
        return StateType::EXIT;
    default:
        break;
    }

    return StateType::NONE;
}