cmake_minimum_required(VERSION 3.7)

project(barnesHut)


set(CMAKE_CXX_STANDARD 20)


find_package(SDL2 REQUIRED)

include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp BHTree.cpp BHTree.h Body.h Quad.cpp Quad.h BHGraphics.cpp BHGraphics.h Simulation.cpp Simulation.h TestManager.cpp TestManager.h)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})