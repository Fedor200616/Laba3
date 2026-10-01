#pragma once

enum class StateType {
    MainMenu,
    Explorer,
    SaveDialog,
    ExitDialog,

    EditorView, //Меню с данными студентов
    EditorMenu, //Меню с изменением данных

    EXIT,

    NONE,
};