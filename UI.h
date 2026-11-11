#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include <Windows.h>
#include <string>

#include "States.h"
#include "File.h"
#include "Student.h"
#include "Main.h"
#include "Log.h"
#include "Input.h"


namespace UI {
    enum class SelType { Selected, Underlined};

    size_t utf8Length(const std::string& str);

    std::string padRight(const std::string& str, size_t width);

    /// <summary>
    /// Вывод на экран отдельной ячейки
    /// </summary>
    /// <param name="ss">поток ввода</param>
    /// <param name="value">текст</param>
    /// <param name="width">Ширина ячейки</param>
    /// <param name="active">Активна ли ячейка</param>
    /// <param name="cell_type">Тип подсветки строки при выборе</param>
    void addCell(
        std::ostringstream& ss,
        const std::string& value,
        size_t width,
        bool active = false,
        SelType cell_type = SelType::Selected
    );

    std::string header(int num_width, const DataBase& data);
}

struct MenuStr {
    std::string name;
    std::function<std::string()> param = nullptr;
    std::function<void()> act_aft = nullptr;
    StateType state_aft = StateType::NONE; //Предполагается, что уже на уровне пункта меню мы будем определять, куда идем после выбора
    
    bool show = true; // Отображать ли пункт меню
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

    std::string before_show;
    std::vector<MenuStr> menu;
    std::string post_show;

    std::string ActMark = "\033[30;47m";
    std::string InactMark = "\033[0m";

    bool tab_ent = false;
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
    
    static MenuNav GetAction(input::key key_code) ;

    virtual bool beforeShow(); //Функция действия перед показом меню
    std::string showUI(); // Функция формирования строки на вывод
    virtual StateType handleNav(); // Функция обработки действий пользователя
    
    static bool windowSize() {
        std::cout << "\033[H";
        return true;
    }
};

//======================Классы наследники меню===========================

//Главное меню, открывается при запуске приложения
class MainMenu : public MenuLogic {
public:
    MainMenu(File& f, DataBase& b);

private:
    File& file;
    DataBase& data;

    void displaySet(bool file_exist);
    std::string path_str() const;

    bool beforeShow() override;

};

//Меню типа да/нет
class YesNoMenu : public MenuLogic {
public:
    YesNoMenu(const std::string &str, const StateType& before);
    YesNoMenu(const std::string &str, const StateType& before, const std::string& enter_str, const std::string& cancel_str);
    bool getRes() const{ return res; }
private:
    StateType bef_state;
    bool res;
    std::string bef_str;
};

//Меню сообщения об ошибке
class NotVerMenu : public MenuLogic {
public:
    NotVerMenu(const std::string &s, const size_t c) : inf(sizeToInf(c)), str(s), val_res(validate::check(inf, str)) {
        LOG_INFO("Меню отображения ошибки");
        LOG_INFO(validate::valMessage(val_res) + " " + str);
        system("cls");
        menu_out.before_show = "Недопустимые символы в введенной строке " + str;
        menu_out.post_show = validate::valMessage(val_res) + "\n";
        menu_out.ActMark = "";
        menu_out.post_show += "Нажмите любую клавишу чтобы вернуться к вводу\n";
        menu_out.before_state = StateType::ANYKEY;
    }
private:
    Inf inf;
    std::string str;
    ValRes val_res;
    StateType handleNav() override {
        bool key = input::getAnyKey();
        system("cls");
        return StateType::ANYKEY;
    }
};

//Меню просмотра файла
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
    std::string createString(size_t i) const;

    bool beforeShow() override;
    StateType handleNav() override;

    const unsigned int MENU_SHOW_LENGTH = 20;
	const int MENU_NUM_TO_SHOW = 3; // Количество строк сверху и снизу от активной, которые будут показаны
};

//Меню дополнительных функций при работе с файлом
class ViewerTab : public MenuLogic {
public:
    ViewerTab(DataBase& db);
private:
    DataBase& data;
    std::string filterShow() const;

    StateType handleNav() override;
};

//Меню изменения выбранного поля
class EditMenu : public MenuLogic {
private:
    DataBase& data;
	Position& pos; //позиция активного поля в меню Viewer, которое мы редактируем

    bool is_mod;

	bool beforeShow() override;
    StateType handleNav() override;

    std::string createString() const;
    
    //Функции работы с БД
    void deleteRow() const;
    enum class AddType {
        Before,
        After,
        End
    };
    void addStr(AddType type) const;

public:
    EditMenu(DataBase& base, Position& db_act);
}; 

//Меня записи нового значения в поле (с клавиатуры)
class EditField : public MenuLogic {
public:
    EditField(DataBase& db, Position& p);
private:
    DataBase& data;
    Position& pos;
    input::TextInput input_line;

    std::string createString() const;
    bool beforeShow() override;
    StateType handleNav() override;
};

//==========================Меню поиска и его производные========================

//Общий (абстрактный) класс поиска
class FindMenuLogic : public MenuLogic { //Абстрактный класс - родитель множества меню поиска и редактирования полей
public:
    FindMenuLogic(DataBase& d, Position& p, const StateType &before_state);
protected:
    DataBase& data;
    Position& pos;

    size_t field_int;
    std::string field;
    std::string find_str;
    std::string repl_str;
    size_t match() const; //совпадения поиска
    std::string line(size_t item, const std::string& value) const;

    bool typing = false;
    std::string* target = nullptr;   // какую строку сейчас набираем
    input::TextInput input_line;

    void startInput(); // Функция печати для поиска
    void changeField(); // функция смены поля при нажатии enter по нему (вместо стрелочек)
    void editAll() const; // Функция замены полей

    bool beforeShow() override;
    StateType handleNav() override;
};

//Класс замены полей по ключевому слову
class ReplaceMenu : public FindMenuLogic {
protected:
    bool beforeShow() override;
public:
    ReplaceMenu(DataBase& d, Position& p, const StateType &before_state);
};

//Класс фильтрации полей
class FilterMenu : public FindMenuLogic {
public:
    FilterMenu(DataBase& d, Position& p, const StateType &before_state);
};

//Класс поиска поля
class FindMenu : public FindMenuLogic {
public: 
    FindMenu(DataBase& d, Position& p, const StateType &before_state);
    Position getPos() const;
private:
    Position pos_of_find;
    std::string createString() const;

    bool beforeShow() override;
};

//=============================Меню проводника и его наследники====================

//Класс выбора файла
class ExplorerMenuLogic : public MenuLogic {
protected:
    static constexpr size_t NAME_WIDTH = 36;// Ограничения чтобы оно не ломало таблицу
    static constexpr size_t MOD_TIME_WIDTH = 16; // 31.08.2026 19:28 -> 2*6 чисел + 4 разделителя
    static constexpr size_t SIZE_WIDTH = 12;
    static constexpr size_t ICON_WIDTH = 2;
    static constexpr size_t CELL_PADDING = 3; // UI::addCell() добавляет по одному пробелу с каждой стороны
    const size_t FULL_WIDTH = NAME_WIDTH + MOD_TIME_WIDTH + SIZE_WIDTH + CELL_PADDING * 3;
    static constexpr size_t MENU_SHOW_LENGTH = 20;
    static constexpr size_t MENU_NUM_TO_SHOW = 3; // Количество строк сверху и снизу от активной, которые будут показаны
    
    DirectoryEntry selected_file;
    Explorer explorer;
    std::vector<DirectoryEntry> entries;
    
    // переменные ниже использовать только после beforeShow()
    size_t first_row = 0;
    size_t last_row = MENU_SHOW_LENGTH;
    size_t real_total = 0;
    size_t real_act = 0;
    bool beforeShow() override;

    std::string createFileStr(size_t index) const;
    void select(size_t i);

    static bool to_show(const PathType type) {
        switch (type) {
        case PathType::HIDE:
        case PathType::OtherFile:
        case PathType::ERR:
        case PathType::NONE:
            return false;
        default:
            return true;
        }
    }
    std::string beforeStr() const;
    void aftEnter();

public:
    explicit ExplorerMenuLogic(const DirectoryEntry& sel); //explicit запрещает преобразования типов
    DirectoryEntry getSelFile() { return selected_file; }
    fs::path getLastPath() const { return explorer.currentPath(); }
    DirectoryEntry getTabEntry() const; // Файл TXT/CSV под курсором, а если его нет - текущая папка
    void goToDir(const fs::path& p); // Перейти в папку (или в папку файла)
};


