#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include <Windows.h>
#include <conio.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iomanip>

#include "States.h"
#include "File.h"
#include "Student.h"
#include "Main.h"
#include "Log.h"
#include "Input.h"


namespace UI {
    enum class SelType { Selected, Yellow, Underlined};

    size_t utf8Length(const std::string& str);

    std::string padRight(const std::string& str, size_t width);

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
        bool active = false,
        SelType cell_type = SelType::Selected
    );

    std::string header(int num_width, StudentDB& data);
}

struct MenuStr {
    std::string name = "";
    bool show = true; // Отображать ли пунтк меню
    bool enter = true; // может ли пользователь выбрать данный пункт меню
    std::function<std::string()> param = nullptr;
    StateType state_aft_ent = StateType::NONE; //Предполагается, что уже на уровне пункта меню мы будем определять, куда идем после выбора
};

struct Position{
    size_t row = 0;
    size_t col = 0;
};

struct MenuOut {
	StateType before_state = StateType::EXIT;

    Position total;
    Position act;

    std::string before_show = "";
    std::vector<MenuStr> menu;
    std::string post_show = "";

    std::string ActMark = "\033[30;47m";
    std::string InactMark = "\033[0m";

    bool tab_ent = 0;
};

enum class MenuNav {
    Left,
    Right,
    Up,
    Down,

    Enter,
    Back,

    KeyTab,
    KeyBackspace,
    Add3,
    Unexpected
};

/// <summary>
/// Базовый абстрактный класс - родитель для любого меню
/// </summary>
class MenuLogic {
public:
    virtual ~MenuLogic() = default;
    
    StateType menuShow();

protected:
    MenuOut menu_out;
    //bool finished = false;
    //StateType next_state;      
    
    MenuNav GetAction(input::key key_code) const;

    virtual bool beforeShow(); //Функция действия перед показом меню
    std::string showUI(); // Функция показа меню
    virtual StateType handleNav(); // Функция обработки действий пользователя
    

    bool windowSize() const {
        // Возвращаем курсор в начало экрана (без сброса экрана и мерцания)
        std::cout << "\033[H";
        //system("cls");
        return true;
    }

};

class Viewer : public MenuLogic {
private:
    StudentDB& data;

    int num_width = 0;
    int menu_width = 0;
public:
    Viewer(StudentDB& students);
	Position getActivePosition() const {
		return menu_out.act;
	}

private: 

    /// <summary>
    /// Функция формирования строки
    /// </summary>
    /// <param name="i">номер строки</param>
    /// <returns>готовая сформированная строка</returns>
    std::string createString(size_t i);
    void updateMenu();

    bool beforeShow() override;

    const unsigned int MENU_SHOW_LENGTH = 20;
	const int MENU_NUM_TO_SHOW = 3; // Количество строк сверху и снизу от активной, которые будут показаны
};

class EditMenu : public MenuLogic {
private:
    StudentDB& data;
	Position pos; //позиция активного поля в меню Viewer, которое мы редактируем

    const std::vector<std::string> menu_str = {
        "",
        "Изменить", // делаем сейчас
        "Заменить поля с данным именем", // примерно после создание основного меню, чтоб вызывалось одно меню, ток  с введенным полем
        "Удалить строку", // делать легко, на уровне функции StudentDB
        "Добавить строку сверху", // делать легко
        "Добавить строку снизу", //делать легко
        "Вернуться к просмотру полей"
    }; // Возможно тут добавить возможность отмены и сохранения изменений
    std::vector<StudentInfo> temp_info;
    bool is_mod;

	bool beforeShow() override;
    StateType handleNav() override;

    std::string createString();
    
public:
    EditMenu(StudentDB& base, Position act);
}; //Кароче перегрузим HandleNav, и в одном режиме мы выбираем базовый, а в другом пишем свой, и все чикибумбони