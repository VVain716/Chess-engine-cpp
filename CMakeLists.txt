cmake_minimum_required(VERSION 3.16)
project(chess-engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

# Core Chess Engine Library (Pure C++, No SDL dependencies)
add_library(chess_core
    src/core/Types.cpp
    src/core/Board.cpp
    src/core/MoveGen.cpp
    src/core/Notation.cpp
)

target_include_directories(chess_core PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(chess_core PRIVATE -Wall -Wextra -Wpedantic)

# UI & Rendering Library (SDL3 & SDL3_image)
add_library(chess_ui
    src/ui/TextureManager.cpp
    src/ui/ChessRenderer.cpp
    src/ui/Animation.cpp
)

target_include_directories(chess_ui PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(chess_ui PUBLIC chess_core SDL3::SDL3 SDL3_image::SDL3_image)
target_compile_options(chess_ui PRIVATE -Wall -Wextra -Wpedantic)
target_compile_definitions(chess_ui PRIVATE CHESS_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

# Main Executable
add_executable(chess-engine src/main.cpp)
target_link_libraries(chess-engine PRIVATE chess_ui chess_core SDL3::SDL3 SDL3_image::SDL3_image)
target_compile_options(chess-engine PRIVATE -Wall -Wextra -Wpedantic)
target_compile_definitions(chess-engine PRIVATE CHESS_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

# Unit Tests Executable
add_executable(test_chess_core tests/test_chess_core.cpp)
target_link_libraries(test_chess_core PRIVATE chess_core)
target_compile_options(test_chess_core PRIVATE -Wall -Wextra -Wpedantic)
