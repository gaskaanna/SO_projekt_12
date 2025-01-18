cmake_minimum_required(VERSION 3.28)
project(SO_projekt C)

#set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)

add_executable(SO_projekt src/main.c
        src/dispenser.c
        include/dispenser.h
        include/baker.h
        src/baker.c
        include/cashier.h
        src/cashier.c
        src/client.c
        include/client.h
)


