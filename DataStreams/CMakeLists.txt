cmake_minimum_required(VERSION 3.10)
project(ne-reader-data-streams)

add_library(data-streams SHARED
    interfaces/libstdc++.h libstdc++.cpp
)