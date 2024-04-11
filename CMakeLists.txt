cmake_minimum_required(VERSION 3.10)

project(HelloWorld)

set(WINDOW_NAME "Hello Opengl")
set(WINDOW_WIDTH 960)
set(WINDOW_HEIGHT 960)

find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(HelloWorld helloworld.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    fmt::fmt
    glfw
 )

 target_compile_definitions(${PROJECT_NAME} PUBLIC
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
 )