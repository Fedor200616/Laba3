cmake_minimum_required(VERSION 4.4)
project(Laba3)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(Laba3
        Application.cpp
        Application.h
        File.cpp
        File.h
        Input.cpp
        Input.h
        Log.h
        Main.cpp
        Main.h
        States.h
        Student.cpp
        Student.h
        UI.cpp
        UI.h)
