cmake_minimum_required(VERSION 3.16)
project(chess-engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Core Chess Engine Library (Pure C++, No SDL dependencies)
add_library(chess_core
    src/core/Types.cpp
    src/core/Board.cpp
    src/core/MoveGen.cpp
    src/core/Notation.cpp
    src/core/Eval.cpp
    src/core/Search.cpp
)

target_include_directories(chess_core PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(chess_core PRIVATE -Wall -Wextra -Wpedantic -O3)

# UCI Engine Executable (Headless, standard chess protocol)
add_executable(chess-uci src/uci.cpp)
target_link_libraries(chess-uci PRIVATE chess_core)
target_compile_options(chess-uci PRIVATE -Wall -Wextra -Wpedantic -O3)

# Optional UI & Graphics Dependencies (SDL3, SDL3_image, OpenCV)
find_package(SDL3 QUIET)
find_package(SDL3_image QUIET)
find_package(OpenCV QUIET)

if(SDL3_FOUND AND SDL3_image_FOUND AND OpenCV_FOUND)
    # UI & Rendering Library (SDL3, SDL3_image & OpenCV)
    add_library(chess_ui
        src/ui/TextureManager.cpp
        src/ui/ChessRenderer.cpp
        src/ui/Animation.cpp
        src/ui/Heatmap.cpp
    )

    target_include_directories(chess_ui PUBLIC ${PROJECT_SOURCE_DIR}/include ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(chess_ui PUBLIC chess_core SDL3::SDL3 SDL3_image::SDL3_image ${OpenCV_LIBS})
    target_compile_options(chess_ui PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_definitions(chess_ui PRIVATE CHESS_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

    # Main Executable
    add_executable(chess-engine src/main.cpp)
    target_link_libraries(chess-engine PRIVATE chess_ui chess_core SDL3::SDL3 SDL3_image::SDL3_image)
    target_compile_options(chess-engine PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_definitions(chess-engine PRIVATE CHESS_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

    # Play vs Computer Executable (with Eval Graph Sidebar)
    add_executable(play-computer src/play-computer.cpp)
    target_link_libraries(play-computer PRIVATE chess_ui chess_core SDL3::SDL3 SDL3_image::SDL3_image)
    target_compile_options(play-computer PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_definitions(play-computer PRIVATE CHESS_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

    # Heatmap Generator Executable
    add_executable(make-heatmaps src/make-heatmaps.cpp)
    target_link_libraries(make-heatmaps PRIVATE chess_ui chess_core SDL3::SDL3 SDL3_image::SDL3_image ${OpenCV_LIBS})
    target_compile_options(make-heatmaps PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_definitions(make-heatmaps PRIVATE CHESS_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
else()
    message(STATUS "GUI dependencies (SDL3, SDL3_image, OpenCV) not found. Building headless UCI engine only.")
endif()