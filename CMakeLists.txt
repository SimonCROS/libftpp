cmake_minimum_required(VERSION 3.22)
project(libftpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(libftpp STATIC
        libftpp.hpp

        library/data_structures.hpp
        library/pool.hpp
        library/data_buffer.cpp
        library/data_buffer.hpp

        library/design_patterns.hpp
        library/memento.cpp
        library/memento.hpp
        library/observer.hpp
        library/singleton.hpp
        library/state_machine.hpp

        library/thread_safe_iostream.cpp
        library/thread_safe_iostream.hpp
        library/prefixed_ostream.cpp
        library/prefixed_ostream.hpp

        library/threading.hpp
        library/thread_safe_queue.hpp
        library/thread.cpp
        library/thread.hpp
        library/worker_pool.cpp
        library/worker_pool.hpp
        library/persistent_worker.cpp
        library/persistent_worker.hpp

        library/mathematics.hpp
        library/mathematics_concepts.hpp
        library/vector2.hpp
        library/vector3.hpp
        library/random_2d_coordinate_generator.cpp
        library/random_2d_coordinate_generator.hpp
        library/perlin_noise_2d.cpp
        library/perlin_noise_2d.hpp

        library/chrono.hpp
        library/chronometer_base.cpp
        library/chronometer_base.hpp
        library/basic_chronometer.hpp
        library/chronometer.cpp
        library/chronometer.hpp
        library/timer.cpp
        library/timer.hpp

        library/observable_value.hpp

        library/expected.hpp

        library/logger.cpp
        library/logger.hpp

        library/internal/compile_helpers.hpp
)
set_target_properties(libftpp PROPERTIES PREFIX "")
target_compile_options(libftpp PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -Wall -Wextra -Werror>
        $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /WX>
)
target_include_directories(libftpp PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/library
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

add_compile_definitions(LIBFTPP_DISABLE_P0323R12)
