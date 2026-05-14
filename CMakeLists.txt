cmake_minimum_required(VERSION 3.16)
project(alice_dungeon_bot CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(ENABLE_SANITIZERS "Build with AddressSanitizer + UndefinedBehaviorSanitizer" OFF)
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(task ${SOURCES})

target_compile_options(task PRIVATE -Wall -Wextra -Wpedantic)

if(WARNINGS_AS_ERRORS)
    target_compile_options(task PRIVATE -Werror)
endif()

if(ENABLE_SANITIZERS)
    target_compile_options(task PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
        -g
    )
    target_link_options(task PRIVATE -fsanitize=address,undefined)
endif()

enable_testing()
add_test(
    NAME run_tests
    COMMAND bash ${CMAKE_SOURCE_DIR}/tests/run_tests.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
set_tests_properties(run_tests PROPERTIES
    ENVIRONMENT "TASK_BIN=$<TARGET_FILE:task>"
)