cmake_minimum_required(VERSION 3.10)
project(NEReader-Core)

include_directories("C:/Program Files/mingw64/include")

add_library(ne-core SHARED
    interfaces/Stream.h
    interfaces/NE_Blocks.h
    interfaces/NEFile.h
    NE_Blocks.cpp
    NEFile.cpp)