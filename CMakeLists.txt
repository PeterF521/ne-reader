cmake_minimum_required(VERSION 3.10)
project(ne-reader)

add_subdirectory(Core)
add_subdirectory(DataStreams)
add_subdirectory(CLI)

target_link_directories(ne-cli PUBLIC data-streams)
target_link_directories(ne-cli PUBLIC ne-core)