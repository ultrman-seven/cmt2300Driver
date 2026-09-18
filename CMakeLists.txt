cmake_minimum_required(VERSION 3.20)

project(CMT2300)
add_library(CMT2300 INTERFACE)

enable_language(C)

target_compile_definitions(CMT2300 INTERFACE
)

target_include_directories(CMT2300 INTERFACE
    ./
)

target_sources(CMT2300 INTERFACE
    ./cmt2300.c
)
