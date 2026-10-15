#pragma once

enum class StateType {
    MainMenu,
    Explorer,
    SaveDialog,
    ExitDialog,

    Viewer, //Меню с отображением таблицы с данными студентов
    
    EditMenu, //Меню с изменением данных
    EditField, // Мен. изменения поля в таблице
    EditAllField, // меню изменения всех полей в таблице с тем же названием

    EXIT,

    NONE,

    _ERROR
};