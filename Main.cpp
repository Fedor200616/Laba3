#include <Windows.h>
#include <iostream>
#include "Main.h"
#include "File.h"
#include "Log.h"
#include "UI.h"
#include "Application.h"


int main(int argc, char* argv[]) {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    Application app(argc, argv);
    app.run();
    return 0;
}
