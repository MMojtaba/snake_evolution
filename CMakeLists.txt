cmake_minimum_required(VERSION 3.26)
project(snake_evolution_game LANGUAGES CXX)

# find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

include_directories(${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} )
link_libraries(${GLEW_LIBRARIES} ${GLFW_LIBRARIES})
add_executable(run_snake_evolution snake_evolution.cpp include/shader_code.hpp)

target_link_libraries(run_snake_evolution glfw)
target_link_libraries(run_snake_evolution GL)
