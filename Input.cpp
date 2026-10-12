#include "Input.h"
#include <Windows.h>
#include <conio.h>

namespace {
    std::string toUtf8(const std::wstring& w) {
        if (w.empty()) return {};
        int n = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
        std::string s(n, '\0');
        WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), s.data(), n, nullptr, nullptr);
        return s;
    }

    std::wstring fromUtf8(const std::string& s) {
        if (s.empty()) return {};
        int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
        std::wstring w(n, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), w.data(), n);
        return w;
    }
}

namespace input {
    /// <summary>
    /// Функция ожидания любой клавиши
    /// </summary>
    /// <returns>true</returns>
    bool getAnyKey() {
        readKeyEvent();
        return true;
    }

    /// <summary>
    /// Получение и преобарзование нажатия в клавише
    /// </summary>
    /// <param name="text_ent">флаг того что вводится текст, чтоб не пропускать 224</param>
    /// <returns>Символ или key от нажатия</returns>
    KeyEvent readKeyEvent() {
        wint_t c = _getwch();

        if (c == 0 || c == 0xE0) {                 // префикс служебной клавиши
            switch (_getwch()) {
            case 72: return { key::Up, 0 };
            case 80: return { key::Down, 0 };
            case 75: return { key::Left, 0 };
            case 77: return { key::Right, 0 };
            case 71: return { key::Home, 0 };
            case 79: return { key::End, 0 };
            case 83: return { key::Delete, 0 };
            default: return {};
            }
        }

        switch (c) {
        case 13: return { key::Enter, 0 };
        case 27: return { key::Esc, 0 };
        case 9:  return { key::Tab, 0 };
        case 8:  return { key::Backspace, 0 };
        default:
            if (c >= 32) return { key::Null, static_cast<wchar_t>(c) };
            return {};
        }
    }


    key getKey() { return readKeyEvent(false).code; }

    // ---------- TextInput ----------

    /// <summary>
    /// Начинает редактирование: загружает исходный текст в буфер и ставит курсор в конец.
    /// </summary>
    /// <param name="utf8">Исходное значение поля (UTF-8)</param>
    /// <param name="max">Максимальная длина текста в символах (без учёта курсора)</param>
    void TextInput::start(const std::string& utf8, size_t max) {
        buf = fromUtf8(utf8);
        pos = buf.size();
        max_len = max;
    }

    /// <summary>
    /// Обрабатывает одно нажатие клавиши: перемещение курсора, удаление, вставку символа.
    /// </summary>
    /// <param name="ev">Событие клавиши (код служебной клавиши или печатный символ)</param>
    /// <returns>
    /// Accepted — нажат Enter, Cancelled — нажат Esc, Editing — ввод продолжается.
    /// </returns>
    TextInput::Result TextInput::handle(const KeyEvent& ev) {
        switch (ev.code) {
        case key::Enter: return Result::Accepted;
        case key::Esc:   return Result::Cancelled;
        case key::Left:  if (pos > 0) --pos; break;
        case key::Right: if (pos < buf.size()) ++pos; break;
        case key::Home:  pos = 0; break;
        case key::End:   pos = buf.size(); break;
        case key::Backspace:
            if (pos > 0) buf.erase(--pos, 1);
            break;
        case key::Delete:
            if (pos < buf.size()) buf.erase(pos, 1);
            break;
        default:
            if (ev.ch != 0
                && forbidden.find(ev.ch) == std::wstring::npos
                && buf.size() < max_len) {
                buf.insert(pos, 1, ev.ch);
                ++pos;
            }
        }
        return Result::Editing;
    }

    /// <summary>
    /// Возвращает введённый текст без курсора.
    /// </summary>
    /// <returns>Текущее содержимое буфера в UTF-8</returns>
    std::string TextInput::text() const { return toUtf8(buf); }

    /// <summary>
    /// Возвращает текст с нарисованным курсором (▌) для отображения в ячейке.
    /// </summary>
    /// <returns>Содержимое буфера с символом курсора в позиции pos (UTF-8)</returns>
    std::string TextInput::display() const {
        std::wstring w = buf;
        w.insert(pos, 1, L'\u258C');   // курсор ▌
        return toUtf8(w);
    }

}