#include "File.h"
#include "Log.h"

namespace {
    // Вспомогательная функция для удаления лишних пробелов по краям поля
    inline std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }
    // Удаление UTF-8 BOM, если он есть
    inline void stripBom(std::string& s) {
        if (s.size() >= 3 &&
            (unsigned char)s[0] == 0xEF &&
            (unsigned char)s[1] == 0xBB &&
            (unsigned char)s[2] == 0xBF) {
            s.erase(0, 3);
        }
    }
    inline bool isHeader(const StudentInfo& r) {
        return r.name == "Имя" &&
            r.group == "Группа" &&
            r.num == "Номер" &&
            r.pass == "Пароль";
    }
}

// Функция копирования строк фаайла в контейнер строки базы данных
StudentInfo File::copyFromString(const std::string& str_buf, unsigned int i, char sep_ch) {
    StudentInfo res = { i, "", "", "", "" }; 
    std::istringstream iss(str_buf); // используем поток ввода строки из файла
    std::string field; // буфер поля в файле

    int field_idx = 0;

    while (std::getline(iss, field, sep_ch)) {
        std::string clean_field = trim(field); // удаляем все лишнее в поле

        //Пропускаем первое пустое поле, если строка начиналась с '|'
        if (clean_field.empty() && field_idx == 0) {
            continue;
        }

        field_idx++; //увеличиваем индекс колонки

        if (clean_field == "Нет данных") {
            clean_field = ""; // Либо оставляем "Нет данных", если так нужно по ТЗ
        }

        switch (field_idx) {
        case 1:
            res.name = clean_field;
            break;
        case 2:
            res.group = clean_field;
            break;
        case 3:
            res.num = clean_field;
            break;
        case 4:
            res.pass = clean_field;
            break;
        default:
            break; // Если полей больше 4, просто игнорируем
        }
    }

    return res;
}

// импорт массива данных из файла
std::vector<StudentInfo> File::loadFromFile() {
    std::vector<StudentInfo> result;
	LOG_INFO("Открытие файла: " + pathToUtf8(filePath));
    bool sep_change = 0; // Флаг смены разделителя, если 1 раз сменился, то не , и не | , то ошибка
    bool is_norm = 0; // Флаг распознания шапки таблицы
    bool first_line = true; // флаг первой строки
    bool header_found = false; // Флаг нахождения заголовка
    if (filePath.empty() || !fs::exists(filePath)) {
        LOG_ERROR("Файл не существует или не найден: " + pathToUtf8(filePath));
        type = fileType::ERR;
        return result;
    }

	std::ifstream ifile(filePath);

	
	std::string str_buf; // промежуточная строка из которой будем брать инфу

	if (!ifile.is_open()) {           // проверяем открыто ли
		LOG_ERROR("Не удалось открыть файл фамилий\n");
        type = fileType::ERR;
		return result;
	}

	unsigned int i = 0;
    char separator = '|';

    while (std::getline(ifile, str_buf)) {
        if (first_line) {
            stripBom(str_buf);
            first_line = false;
        }

        if (trim(str_buf).empty())
            continue;

        // Первая непустая строка: определяем разделитель по шапке
        if (!header_found) {
            const char separators[] = { '|', ',' };
            bool ok = false;

            for (char sep : separators) {
                LOG_INFO(std::string("Попытка анализа с разделителем ") + sep);
                StudentInfo hdr = copyFromString(str_buf, 0, sep);
                if (isHeader(hdr)) {
                    separator = sep;
                    ok = true;
                    break;
                }
            }

            if (!ok) {
                LOG_ERROR("Ошибка в шапке файла");
                type = fileType::ERR;
                return {};
            }

            header_found = true;
            i = 1;
            continue; // шапку в вектор не записываем
        }

        StudentInfo res_buf = copyFromString(str_buf, i, separator);
        res_buf.row = i;
        result.push_back(res_buf);
        ++i;
    }
    LOG_INFO("Файл обработан");
	LOG_INFO("Количество записей: " + std::to_string(result.size()));
    return result;
}