cmake_minimum_required(VERSION 3.29)
project(libftpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(libftpp STATIC
        library/pool.hpp
        library/data_buffer.cpp
        library/data_buffer.hpp
)
target_include_directories(libftpp PUBLIC
        library
)

function(create_test_executables FOLDER_NAME TARGET_PREFIX)
    # Get all tests and make a rule for each of them
    set(TESTS_DIR ${CMAKE_SOURCE_DIR}/${FOLDER_NAME})
    file(GLOB TEST_SOURCES "${TESTS_DIR}/main_*.cpp")
    foreach (TEST_SOURCE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

        set(TARGET_NAME ${TARGET_PREFIX}${TEST_NAME})

        add_executable(${TARGET_NAME} ${TEST_SOURCE})
        target_link_libraries(${TARGET_NAME} libftpp)
    endforeach ()
endfunction()

create_test_executables(42tests "42_")
create_test_executables(tests "")
