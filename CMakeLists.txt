cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(snake_evolution_game LANGUAGES CXX)

# find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

#add and link glfw and glew libraries
include_directories(${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} )
link_libraries(${GLEW_LIBRARIES} ${GLFW_LIBRARIES})

#create executable and link with libraries
add_executable(run_snake_evolution snake_evolution.cpp include/shader_code.hpp include/program.hpp include/game.hpp include/utils.hpp)
target_link_libraries(run_snake_evolution glfw)
target_link_libraries(run_snake_evolution GL)

#install the program
install(TARGETS run_snake_evolution DESTINATION bin)

#install demo script
install(PROGRAMS demo DESTINATION bin)
