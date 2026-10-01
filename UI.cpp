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


/// <summary>
/// Вывод на экран отдельной ячейки
/// </summary>
/// <param name="ss">поток ввода</param>
/// <param name="value">текст</param>
/// <param name="width">Ширина ячейки</param>
/// <param name="active">Активна ли ячейка</param>
static void addCell(std::ostringstream& ss,
    const std::string& value,
    int width,
    bool active)
{
    if (active)
        ss << "\033[97m";

    ss << " "
        << std::left
        << std::setw(width)
        << value
        << " ";

    if (active)
        ss << "\033[0m";

    ss << "|";
}


std::string Editor::createString(int row)
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
	unsigned int num_width = std::to_string(menu_out.total.row).length();
	addCell(
		ss,
		"№",
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