#pragma once
#include <string>

namespace input {

    enum class key {
        Up = 72, Down = 80, Left = 75, Right = 77,
        Home = 71, End = 79, Delete = 83,

        Enter = 13, Esc = 27, Tab = 9, Backspace = 8,
        Modif = 42,

        Extended = 224,
        Null = 0
    };

    struct KeyEvent {
        key code = key::Null;
        wchar_t ch = 0;          // печатный символ, если это он
    };

    bool getAnyKey();
    KeyEvent readKeyEvent();     // читает одно нажатие (блокирующий)
    key getKey();                // обёртка: только код клавиши

    /// <summary>
    /// Однострочный текстовый ввод: буфер, курсор, правка.
    /// </summary>
    class TextInput {
    public:
        /// Результат обработки одного нажатия
        enum class Result {
            Editing,    // ввод продолжается
            Accepted,   // нажат Enter, текст можно сохранять
            Cancelled   // нажат Esc, изменения нужно отбросить
        };

        /// Начать редактирование: загрузить исходный текст, курсор в конец
        void start(const std::string& utf8, size_t max_len);

        /// Обработать нажатие клавиши (перемещение курсора, удаление, вставка символа)
        Result handle(const KeyEvent& ev);

        std::string text() const;      // итоговый текст без курсора (UTF-8)
        std::string display() const;   // текст с курсором ▌ для отрисовки в ячейке

        std::wstring forbidden = L"|,"; // символы, которые нельзя вводить (разделители файла)

    private:
        std::wstring buf;      // буфер ввода (wstring: символ = 1 элемент, удобно для кириллицы)
        size_t pos = 0;        // позиция курсора в буфере (0 = перед первым символом)
        size_t max_len = 0;    // максимальная длина текста в символах
    };

}