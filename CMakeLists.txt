cmake_minimum_required(VERSION 3.2)

project(
    gif
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)

set(SRC
    "src/main.c"
    "src/endian.c"
)

add_executable(${CMAKE_PROJECT_NAME} ${SRC})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include")
