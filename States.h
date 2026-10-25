#pragma once

enum class StateType {
    MainMenu,
    Explorer,
    SaveDialog,
    ExitDialog,

    Viewer, //Меню с отображением таблицы с данными студентов
    ViewerTab, // Меню при нажатии таб в viewer
    
    EditMenu, //Меню с изменением данных
    EditField, // Мен. изменения поля в таблице
    ReplaceAll, // меню изменения всех полей в таблице с тем же названием
    FilterMenu, // Меню изменения параметров фильтра

    EXIT,

    NONE,
    ANYKEY, // Специальный стейт, который никак не обработается, аналог NONE, но происходит выход из цикла

    _ERROR
};