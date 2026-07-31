cmake_minimum_required(VERSION 3.16)
project(chess-engine CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

add_library(helper src/board.cpp src/pieces.cpp src/pixel.cpp src/animate.cpp src/moves.cpp src/notate.cpp)

target_include_directories(helper PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(helper PUBLIC SDL3::SDL3 SDL3_image::SDL3_image)

add_executable(chess-engine src/main.cpp)

target_link_libraries(chess-engine PRIVATE helper)
