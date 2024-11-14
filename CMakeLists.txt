cmake_minimum_required(VERSION 3.29)
project(libftpp)

set(CMAKE_CXX_STANDARD 23)

add_library(libftpp STATIC
        library/pool.hpp
        library/data_buffer.cpp
        library/data_buffer.hpp
)
target_include_directories(libftpp PUBLIC
        library
)

add_executable(main
        main.cpp
)
target_link_libraries(main PRIVATE libftpp)
