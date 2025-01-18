cmake_minimum_required(VERSION 3.28)
project(SO_projekt C)

set(CMAKE_C_STANDARD 11)

add_executable(SO_projekt src/main.c
        src/Product.c
        include/Product.h
        src/dispenser.c
        include/dispenser.h
        include/baker.h
        src/baker.c
        include/cash_register.h
        src/cash_register.c
        src/client.c
        include/client.h
)
