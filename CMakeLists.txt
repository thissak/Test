cmake_minimum_required(VERSION 3.10)

project(HelloWorld)

# Set C++17 as the standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(WINDOW_NAME "Hello Opengl")
set(WINDOW_WIDTH 1280)
set(WINDOW_HEIGHT 960)

find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(${PROJECT_NAME} 
   src/main.cpp
   src/common.cpp src/common.h
   src/shader.cpp src/shader.h
   src/program.cpp src/program.h
   src/context.cpp src/context.h
   src/buffer.cpp src/buffer.h
   src/vertexLayout.cpp src/vertexLayout.h
   src/image.h src/image.cpp
   src/texture.h src/texture.cpp
   )

target_link_libraries(${PROJECT_NAME} PRIVATE 
    fmt::fmt
    glfw
    glad::glad
    Eigen3::Eigen
    glm::glm
 )
target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})

target_compile_definitions(${PROJECT_NAME} PUBLIC
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
 )

 