cmake_minimum_required(VERSION 4.0)
project(opengl_test)

set(CMAKE_CXX_STANDARD 17)

# Source files
set(SOURCES
    src/main.cpp
    src/glad.c
    src/shader.cpp
    src/core/camera.cpp
    src/core/worldGenerator.cpp
    src/core/world.cpp
    src/rendering/chunkRenderer.cpp
    src/util/OpenSimplexNoise.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/libglfw3dll.a
)