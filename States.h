#pragma once

enum class StateType {
    MainMenu,
    Explorer,
    SaveDialog,
    ExitDialog,

    Viewer, //Меню с данными студентов
    ViewerMenu, //Меню с изменением данных

    EXIT,

    NONE,

    _ERROR
};