cmake_minimum_required(VERSION 3.28)
project(SO_projekt C)

set(CMAKE_C_STANDARD 11)

add_executable(SO_projekt main.c
        podajnik.c
        Product.c
        Product.h)
