cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(snake_evolution_game LANGUAGES CXX)

# find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

#set include directories to make includes easier
include_directories(${CMAKE_SOURCE_DIR}/include)

#add and link glfw and glew libraries
include_directories(${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} )
link_libraries(${GLEW_LIBRARIES} ${GLFW_LIBRARIES})

#Add header files to a variable
set(HEADERS
        include/game.hpp 
        include/image_data.hpp
        include/program.hpp
        include/texture.hpp
        include/utils.hpp)
#Add source files to a variable
set(SOURCES
        lib/game.cpp
        lib/program.cpp
        lib/texture.cpp)

#create executable and link with libraries
add_executable(run_snake_evolution snake_evolution.cpp ${HEADERS} ${SOURCES})
target_link_libraries(run_snake_evolution glfw)
target_link_libraries(run_snake_evolution GL)

#install the program
install(TARGETS run_snake_evolution DESTINATION bin)

#install demo script
install(PROGRAMS demo DESTINATION bin)
