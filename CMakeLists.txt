cmake_minimum_required(VERSION 3.26)
project(snake_evolution_game LANGUAGES CXX)

# find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

#${OPENGL_INCLUDE_DIRS}
#${OPENGL_LIBRARIES}
include_directories(${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} )
link_libraries(${GLEW_LIBRARIES} ${GLFW_LIBRARIES})
add_executable(run_snake_evolution snake_evolution.cpp)

target_link_libraries(run_snake_evolution glfw)
target_link_libraries(run_snake_evolution GL)

add_executable(test_opengl test_opengl.cpp)
target_link_libraries(test_opengl glfw)
target_link_libraries(test_opengl GL)

add_executable(test_opengl2 test_opengl2.cpp)
target_link_libraries(test_opengl2 glfw)
target_link_libraries(test_opengl2 GL)

