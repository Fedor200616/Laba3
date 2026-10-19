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

    std::string header(int num_width, DataBase& data);
}

struct MenuStr {
    std::string name = "";
    std::function<std::string()> param = nullptr;
    std::function<void()> act_aft = nullptr;
    StateType state_aft = StateType::NONE; //Предполагается, что уже на уровне пункта меню мы будем определять, куда идем после выбора
    
    bool show = true; // Отображать ли пунтк меню
    bool enter = true; // может ли пользователь выбрать данный пункт меню
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

    Position getActivePosition() const {
        return menu_out.act;
    }

protected:
    MenuOut menu_out;   
    
    MenuNav GetAction(input::key key_code) const;

    virtual bool beforeShow(); //Функция действия перед показом меню
    std::string showUI(); // Функция формирования строки на вывод
    virtual StateType handleNav(); // Функция обработки действий пользователя
    
    bool windowSize() const {
        std::cout << "\033[H";
        return true;
    }
};

class Viewer : public MenuLogic {
private:
    DataBase& data;

    int num_width = 0;
    int menu_width = 0;

    size_t first_row = 0;
public:
    Viewer(DataBase& students, Position db_act);

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
    DataBase& data;
	Position& pos; //позиция активного поля в меню Viewer, которое мы редактируем

    bool is_mod;

	bool beforeShow() override;
    StateType handleNav() override;

    std::string createString();
    
    //Функции работы с БД
    void deleteRow();
    enum class AddType {
        Before,
        After,
        End
    };
    void addStr(AddType type);

public:
    EditMenu(DataBase& base, Position& db_act);
}; 


class EditField : public MenuLogic {
public:
    EditField(DataBase& db, Position& p);
private:
    DataBase& data;
    Position& pos;
    input::TextInput input_line;

    std::string createString();
    StateType handleNav() override;
};


class EditAllField : public MenuLogic { //Абстрактный класс - родитель множества меню поиска и редактирования полей
public:
    EditAllField(DataBase& d, Position& p);
protected:
    DataBase& data;
    Position& pos;

    size_t field_int;
    std::string field;
    std::string find_str;
    std::string repl_str = "";
    size_t match() const; //совпадения поиска
    std::string line(size_t item, const std::string& value) const;

    bool typing = false;
    std::string* target = nullptr;   // какую строку сейчас набираем
    input::TextInput input_line;

    void startInput(); // Функция печати для поиска
    void changeField(); // функция смены поля при нажатии enter по нему (вместо стрелочек)

    bool beforeShow() override;
    StateType handleNav() override;
};
