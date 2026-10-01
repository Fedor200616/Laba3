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
}




StudentInfo File::copyFromString(const std::string& str_buf, unsigned int i) {
    StudentInfo res = { i, "", "", "", "" };
    std::istringstream iss(str_buf);
    std::string field;

    int field_idx = 0;

    char sep_ch;
    if (type == fileType::TXT) {
        LOG_INFO("Выбран файл ТХТ, знак раздела - |");
        sep_ch = '|';
    }
    else if (type == fileType::CSV) {
        LOG_INFO("Выбран файл CSV, знак раздела - ,");
        sep_ch = ',';
    }
    else {
        LOG_WARN("Ошибка выбора расширения, знак раздела - |");
        sep_ch = '|';
    }

    while (std::getline(iss, field, sep_ch)) {
        std::string clean_field = trim(field);

        //Пропускаем первое пустое поле, если строка начиналась с '|'
        if (clean_field.empty() && field_idx == 0) {
            continue;
        }

        field_idx++; //ОБЯЗАТЕЛЬНО увеличиваем индекс колонки!

        if (clean_field == "Нет данных") {
            clean_field = "-"; // Либо оставляем "Нет данных", если так нужно по ТЗ
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

std::vector<StudentInfo> File::loadFromFile() {
	std::ifstream ifile(filePath);

	std::vector<StudentInfo> result;
	std::string str_buf; // промежуточная строка из которой будем брать инфу

	if (!ifile.is_open()) {           // проверяем
		LOG_ERROR("Не удалось открыть файл фамилий\n");
        type = fileType::ERROR;
		return result;
	}

	unsigned int i = 0;
	while (std::getline(ifile, str_buf)) {
		StudentInfo res_buf = copyFromString(str_buf, i);
        if (i == 0) {
            bool is_norm = (res_buf.name == "Имя" &&
                res_buf.group == "Группа" &&
                res_buf.num == "Номер" &&
                res_buf.pass == "Пароль");
            if (!is_norm) {
                LOG_ERROR("Ошибка в шапке файла");
                result[0] = { 1, "", "", "", "" };
                type = fileType::ERROR;
                return result;
            }
        }
        result.push_back(res_buf);
        i++;
	}
    LOG_INFO("Файл обработан");
    return result;
}