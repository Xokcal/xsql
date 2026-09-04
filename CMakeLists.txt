cmake_minimum_required(VERSION 3.27)
project(XokSqlC C)

set(CMAKE_C_STANDARD 11)

add_executable(XokSqlC main.c
        file/file.c
        xstr/String.c
        xstr/String.c
        xstr/String.c
        file/file.c)
add_executable(new_target xstr/String.c
        xsqlg/grm.c
        xsqlg/grm.h
        list/list.c
        list/list.h
        cpp_test/main.cpp
        time/xtime.c
        time/xtime.h)
