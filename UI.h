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

namespace input {
    enum class key
    {
        Up = 72,
        Down = 80,
        Left = 75,
        Right = 77,

        Enter = 13,
        Esc = 27,

        Tab = 9,
        Modif = 42, // Левый шифт

        Extended = 224,
        Null = 0
    };

    inline bool getAnyKey(){
        while(_getch() == static_cast<int>(key::Extended)){}
        return true;
    }

    inline key getKey(int ch = _getch()) {

        // Если считн служебный байт стрелок/расширенных клавиш (0 или 224)
        if (ch == 0 || ch == static_cast<int>(key::Extended)) {
            ch = _getch(); // Читаем второй байт с реальным кодом стрелки
        }

        // Преобразуем код в enum
        switch (ch) {
        case static_cast<int>(key::Up):    return key::Up;
        case static_cast<int>(key::Down):  return key::Down;
        case static_cast<int>(key::Left):  return key::Left;
        case static_cast<int>(key::Right): return key::Right;
        case static_cast<int>(key::Enter): return key::Enter;
        case static_cast<int>(key::Esc):   return key::Esc;
        case static_cast<int>(key::Tab):   return key::Tab;
        default:                           return key::Null;
        }
    }

}

struct MenuStr {
    std::string name;
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

    std::string ActMark = "->";
    std::string InactMark = "  ";

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

    std::string showUI(); // Функция показа меню
    virtual StateType handleNav(); // Функция обработки действий пользователя
    
    virtual bool beforeShow() = 0; //Функция действия перед показом меню

    bool windowSize() const {
        // Возвращаем курсор в начало экрана (без сброса экрана и мерцания)
        std::cout << "\033[H";
        //system("cls");
        return true;
    }

};

class Editor : public MenuLogic {
private:
    StudentDB& data;

    int num_width = 0;
    int menu_width = 0;
public:
    Editor(StudentDB& students);
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
	std::string header(); //функция формирования шапки таблицы
    void updateMenu();

    bool beforeShow() override;

    const unsigned int MENU_SHOW_LENGTH = 20;
	const int MENU_NUM_TO_SHOW = 3; // Количество строк сверху и снизу от активной, которые будут показаны
};

class EditMenu : public MenuLogic {
private:
    StudentDB& data;
    Position pos;

    const std::string menu_str[] = "Изменить",
                                "Заменить поля с данным именем",
                                "Удалить строку",
                                "Добавить строку сверху",
                                "Добавить строку снизу",
                                "Отменить изменения",
                                "Сохранить изменения"
    std::vector<StudentInfo> temp_info;
    bool is_mod = False;

public:
    ~EditMenu();
    EditMenu(StudentDB& base, Position act);
    bool beforeShow() override;
}