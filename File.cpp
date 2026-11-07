#include "File.h"
#include "Student.h"
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
            static_cast<unsigned char>(s[0]) == 0xEF &&
            static_cast<unsigned char>(s[1]) == 0xBB &&
            static_cast<unsigned char>(s[2]) == 0xBF) {
            s.erase(0, 3);
        }
    }
    inline bool isHeader(const StudentInfo& r) {
        return r.name == "Имя" &&
            r.group == "Группа" &&
            r.num == "Номер" &&
            r.pass == "Пароль";
    }
    // Дополнение строки пробелами справа до width СИМВОЛОВ (setw считает байты и ломает кириллицу)
    inline std::string padChars(const std::string& s, size_t width) {
        size_t len = 0;
        for (unsigned char c : s)
            if ((c & 0xC0) != 0x80) // считаем только начала символов UTF-8
                ++len;
        return len >= width ? s : s + std::string(width - len, ' ');
    }

    // Шапка TXT
    inline std::string headerTXT(char sep) {
        return padChars("Имя", 20) + sep
            + padChars("Группа", 12) + sep
            + padChars("Номер", 5) + sep
            + padChars("Пароль", 20) + "\n";
    }

    // Шапка CSV
    inline std::string headerCSV() {
        return "Имя;Группа;Номер;Пароль\n";
    }
}

DirectoryEntry::DirectoryEntry(const fs::path &f_path, PathType f_type): path(f_path), file_type(f_type) {
    init();
}

// Функция копирования строк файла в контейнер строки базы данных
StudentInfo File::copyFromString(const std::string& str_buf, const unsigned int i, const char sep_ch) {
    StudentInfo res = { i, "", "", "", "" }; 
    std::istringstream iss(str_buf); // используем поток ввода строки из файла
    std::string field; // буфер поля в файле

    int field_idx = 0;

    while (std::getline(iss, field, sep_ch)) {
        std::string clean_field = trim(field); // удаляем все лишнее в поле

        field_idx++; //увеличиваем индекс колонки

        if (clean_field == FILE_NODATA) {
            clean_field = VECTOR_NODATA; //заменяем Нет Данных в таблице
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

bool File::checkHeader() {
    if (filePath.empty() || !fs::exists(filePath))
        return false;

    std::ifstream file(filePath);
    if (!file.is_open())
        return false;

    std::string line;
    if (!std::getline(file, line))
        return false;

    stripBom(line);
    line = trim(line);

    // Проверяем оба поддерживаемых разделителя
    for (char sep : { '|', ',', ';' }) {
        StudentInfo header = copyFromString(line, 0, sep);

        if (isHeader(header)) {
            separator = sep;
            return true;
        }
    }

    return false;
}

bool File::setFile(const DirectoryEntry& de) {
    if (de.path.empty())
        return false;

    PathType new_type = de.type();

    if (new_type != PathType::CSV && new_type != PathType::TXT)
        return false;

    std::error_code ec;
    if (!fs::is_regular_file(de.path, ec))
        return false;

    filePath = de.path;
    type = new_type;
    have_marker = false;

    LOG_INFO("Рабочий файл изменён: " + pathToUtf8(filePath));
    return true;
}

// импорт массива данных из файла
std::vector<StudentInfo> File::loadFromFile(){
    std::vector<StudentInfo> result;
	LOG_INFO("Открытие файла: " + pathToUtf8(filePath));
    bool first_line = true; // флаг первой строки
    bool header_found = false; // Флаг нахождения заголовка
    if (filePath.empty() || !fs::exists(filePath)) {
        LOG_ERROR("Файл не существует или не найден: " + pathToUtf8(filePath));
        type = PathType::ERR;
        return {};
    }

	std::ifstream ifile(filePath);

	std::string str_buf; // промежуточная строка из которой будем брать инфу

	if (!ifile.is_open()) {           // проверяем открыто ли
		LOG_ERROR("Не удалось открыть файл фамилий\n");
        type = PathType::ERR;
	    return {};
	}

	unsigned int i = 0;

    while (std::getline(ifile, str_buf)) {
        if (first_line) {
            stripBom(str_buf);
            first_line = false;
        }

        if (trim(str_buf).empty())
            continue;

        // Первая непустая строка: определяем разделитель по шапке
        if (!header_found) {
            header_found = checkHeader();
            if (header_found)
                continue;
            else { //Если первая строка в файле не шапка
                LOG_ERROR("Некорректный заголовок файла");
                type = PathType::ERR;
                return {};
            }
        }

        StudentInfo res_buf = copyFromString(str_buf, i, separator);
        res_buf.row = i;

        result.push_back(res_buf);
        ++i;
    }
    if (!header_found) {
        LOG_WARN("В обработанном файле отсутствует шапка");
        type = PathType::ERR;
        return {};
    }
    LOG_INFO("Файл обработан");
	LOG_INFO("Количество записей: " + std::to_string(result.size()));
    return result;
}

// Сохранение базы данных в файл
bool File::saveToFile(DataBase& db, const DirectoryEntry& dir) {
    // Тип берем по расширению, а не по dir.type(): файла может еще не быть на диске
    const fs::path ext = dir.path.extension();
    const bool is_csv = (ext == ".csv");
    if (!is_csv && ext != ".txt") {
        LOG_ERROR("Ошибка типа файла при сохранении: " + pathToUtf8(dir.path));
        return false;
    }

    std::ofstream out(dir.path, std::ios::trunc);
    if (!out.is_open()) {
        LOG_ERROR("Не удалось открыть файл для записи: " + pathToUtf8(dir.path));
        return false;
    }

    out << (is_csv ? headerCSV() : headerTXT(separator)); // Шапка такая же, как у созданных файлов

    for (const StudentInfo& s : db.getInfoVec()) { //Заполняем основные поля
        // Пустые поля пишем как "Нет данных", loadFromFile превращает их обратно в пустые,
        // иначе пустое первое поле в TXT сдвинуло бы все колонки
        const std::string name = s.name.empty() ? FILE_NODATA : s.name;
        const std::string group = s.group.empty() ? FILE_NODATA : s.group;
        const std::string num = s.num.empty() ? FILE_NODATA : s.num;
        const std::string pass = s.pass.empty() ? FILE_NODATA : s.pass;

        if (is_csv)
            out << name << ';' << group << ';' << num << ';' << pass << '\n';
        else
            out << padChars(name, 20) << separator
            << padChars(group, 12) << separator
            << padChars(num, 5) << separator
            << padChars(pass, 20) << '\n';
    }

    out.close();
    if (out.fail()) {
        LOG_ERROR("Ошибка записи в файл: " + pathToUtf8(dir.path));
        return false;
    }

    LOG_INFO("Данные сохранены в файл: " + pathToUtf8(dir.path) + ", записей: " + std::to_string(db.size()));
    setFile(DirectoryEntry(dir.path)); // Рабочим файлом становится сохраненный
    return true;
}

void Explorer::fillFiles() {
    if (!fs::exists(curr_path) || !fs::is_directory(curr_path)) {
        LOG_WARN("Ошибка формирования вектора файлов в директории " + pathToUtf8(curr_path));
        return;
    }
    file_arr.clear();
    if (!default_path.empty()) {
        const auto home_path = fs::is_directory(default_path)
            ? default_path
            : default_path.parent_path();

        file_arr.emplace_back(home_path, PathType::BackToHome);
    }
    if (curr_path != curr_path.root_path()) {
        file_arr.emplace_back(curr_path.parent_path(), PathType::BackDir); // добавляем кнопку на директорию выше
    }

    for (const auto& entry : fs::directory_iterator(curr_path)) { //Построчечно проходимся по файлам в директории
         file_arr.emplace_back(entry.path());
    }
}

std::vector<DirectoryEntry> Explorer::fileArr() const {
    if(!file_arr.empty())
        return file_arr;
    else {
        LOG_WARN("В директории отсутствует информация Explorer::fileArr()");
        DirectoryEntry parent_path(curr_path.parent_path(), PathType::BackDir);
        std::vector<DirectoryEntry> arr;
        arr.push_back(parent_path);
        return arr;
    }
}

OpenResult Explorer::openPath(const DirectoryEntry& entry) {
    auto path = entry.path;
    std::error_code ec;
    if (!path.empty() && fs::is_regular_file(path, ec))
        return OpenResult::Failed;
    switch (entry.type()) {
    case PathType::Folder:
        curr_path = path;
        fillFiles();
        return OpenResult::DirectoryChanged;
    case PathType::BackDir:
        if (curr_path != curr_path.root_path()) {
            curr_path = curr_path.parent_path();
            fillFiles();
        }
        return OpenResult::DirectoryChanged;
    case PathType::BackToHome:
        curr_path = fs::is_directory(default_path)
            ? default_path
            : default_path.parent_path();

        fillFiles();
        return OpenResult::DirectoryChanged;
    case PathType::TXT:
    case PathType::CSV:
        selected_entry = entry;
        return OpenResult::FileSelected;
    default:
        return OpenResult::Failed;
    }
    return OpenResult::Failed; // на всякий случай
}
