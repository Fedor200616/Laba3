#pragma once

enum class StateType {
    MainMenu,
    OpenExplorer,
    OpenExplorerTabLogic,
    SaveDialog,
    ExitDialog,

    Viewer, //Меню с отображением таблицы с данными студентов
    ViewerTab, // Меню при нажатии таб в viewer
    
    EditMenu, //Меню с изменением данных
    EditField, // Мен. изменения поля в таблице
    ReplaceAll, // меню изменения всех полей в таблице с тем же названием
    FilterMenu, // Меню изменения параметров фильтра
    FindMenu, // Меню поиска студента

    ExplorerOutFile, //Выбранный файл подходит по формату

    EXIT,

    NONE,
    ANYKEY, // Специальный стейт, который никак не обработается, аналог NONE, но происходит выход из цикла

    _ERROR
};