cmake_minimum_required(VERSION 3.23)
project(pdu C)

set(CMAKE_C_STANDARD 99)

add_executable(pdu main.c pdu.c pdu.h)
