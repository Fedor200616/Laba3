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


public:


};


class MenuInfo {
public:
    std::string name;
    bool show = true;
    bool entered = true;
    std::function<std::string()> param = nullptr;
};

struct MenuOut {
    int total_row;
    int act_row;

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

    Add1,
    Add2,
    Add3
};

/// <summary>
/// Базовый абстрактный класс - родитель для любого меню
/// </summary>
class MenuLogic {
public:
    virtual ~MenuLogic() = default;
    
    virtual void beforeShow(MenuOut& menu) {};
    virtual void showUI(const MenuOut& menu);
    virtual void handleNav(MenuNav nav) = 0;

protected:
    MenuOut out;
    bool finished = false;
    StateType next_state;

    bool windowSize() const;
    MenuNav GetAction(input::key key_code) const;
    virtual std::string menuString() = 0;
    
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
    
public:
    Editor(UI_Interface& filepath)

}

