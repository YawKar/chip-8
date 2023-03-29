cmake_minimum_required(VERSION 3.15)
project(chip8 CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(fmt REQUIRED)

# Add Chip8
set(CHIP8 src/chip8/Chip8.cpp)

# Combine sources
set(SRCS src/main.cpp ${CHIP8})

# Make executable
add_executable(${PROJECT_NAME} ${SRCS})

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion)
endif()

target_link_libraries(${PROJECT_NAME} fmt::fmt)
