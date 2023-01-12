cmake_minimum_required(VERSION 3.24)
project(pong)

set(CMAKE_CXX_STANDARD 17)

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})


link_directories(${X11_LIBRARIES})


add_executable(${PROJECT_NAME} ../tetris/src/olcPixelGameEngine.hpp src/main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC "src")
target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES} GL png stdc++fs pthread)