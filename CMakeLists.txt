cmake_minimum_required(VERSION 3.26)
project(snake_evolution_game LANGUAGES CXX)

# find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

#${OPENGL_INCLUDE_DIRS}
#${OPENGL_LIBRARIES}
include_directories(${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES} ${GLFW_LIBRARIES})
add_executable(run_snake_evolution snake_evolution_game.cpp)

target_link_libraries(run_snake_evolution glfw)
