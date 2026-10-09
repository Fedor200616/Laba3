#include <Windows.h>
#include <iostream>
#include "Main.h"
#include "File.h"
#include "Log.h"
#include "UI.h"
#include "Application.h"


int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8); 
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    Application app(argc, argv);
    app.run();
    return 0;
}
