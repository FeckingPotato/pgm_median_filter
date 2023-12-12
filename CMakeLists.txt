cmake_minimum_required(VERSION 3.26)
project(median_filter C)

set(CMAKE_C_STANDARD 11)

add_executable(median_filter
        main.c
        pgm.c
        pgm.h
        filter.c
        filter.h)
