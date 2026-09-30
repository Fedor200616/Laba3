#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include <conio.h>
#include "File.h"
#include "Application.h"
#include "Main.h"

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

class UI_Interface {
private:
    StateType& state;
    Info& app_info;
    
    
public:
    UI_Interface(Info& inf, StateType& s) : app_info(inf), state(s){};

    StateType getState(){
        return state;
    }
    Info getInfo(){
        return info;
    }
    void changeState(StateType new_state){
        state = new_state
    }

    void show(); //Функция отображения нужного пункта меню она не должна отвечать за смену логики

};

class MenuInfo {
public:
    std::string name;
    bool show = true;
    bool entered = true;
    std::function<std::string()> param = nullptr;
};

struct Position{
    unsigned int row = 0;
    unsigned int col = 0;
};

struct MenuOut {
    position total;
    position act;

    std::string before_show = "";
    std::vector<MenuInfo> menu;
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
    Back

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
    MenuOut& menu_out;
    //bool finished = false;
    //StateType next_state;

    bool windowSize() const;
    MenuNav GetAction(input::key key_code) const;
    virtual std::string menuString() = 0;
    
    virtual bool beforeShow() = 0;
    virtual bool showUI() = 0;
    virtual StateType handleNav() = 0;

};

/*
class MainMenu : public MenuLogic {
private:

public:
    MainMenu(UI_Interface& filepath);

    void beforeShow(MenuOut& menu) override;
    void showUI(const MenuOut& menu) override;
    void handleNav(MenuNav nav) override;
};

class ExplorerMenu : public MenuLogic {
private:
    

public:
    ExplorerMenu(UI_Interface& state);

    void beforeShow(MenuOut& menu) override;
    void showUI(const MenuOut& menu) override;
    void handleNav(MenuNav nav) override;

};
*/

class Editor : public MenuLogic {
private:
    UI_Interface& ui;
    StudentDB& data_base;


public:
    Editor(UI_Interface& interface, StudentDB& students) : ui(interface), data_base(students){};

protected:
    bool beforeShow() override;
    bool showUI() override;
    StateType handleNav() override;
}

