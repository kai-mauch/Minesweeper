cmake_minimum_required(VERSION 3.28)
project(project3 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(project3
        src/main.cpp
        src/Tile.cpp
        src/Board.cpp)
target_compile_features(project3 PRIVATE cxx_std_17)
target_link_libraries(project3 PRIVATE SFML::Graphics)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT project3)