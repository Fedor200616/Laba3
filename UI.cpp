#include "UI.h"


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
        return MenuNav::Tab;

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