cmake_minimum_required(VERSION 3.30.5)
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
    src/core/chunkLoader.cpp
    src/rendering/chunkRenderer.cpp
    src/util/OpenSimplexNoise.cpp
        src/rendering/DrawCmd.h
        src/rendering/VertexArena.h
        src/gl/GLBuffer.cpp
        src/gl/GLBuffer.h
        src/gl/VertexArray.cpp
        src/gl/VertexArray.h
        src/gl/Mesh.cpp
        src/gl/Mesh.h
        src/gl/Texture2D.cpp
        src/gl/Texture2D.h
        src/rendering/Crosshair.h
        src/rendering/BlockOutline.cpp
        src/rendering/BlockOutline.h
        src/core/Raycast.cpp
        src/core/Raycast.h
        src/gl/Texture2DArray.cpp
        src/gl/Texture2DArray.h
        src/util/paths.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

# static mingw runtime
if (MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ -static)
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Resource dir
target_compile_definitions(${PROJECT_NAME} PRIVATE
        RESOURCE_DIR="${CMAKE_SOURCE_DIR}/resources")

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/libglfw3dll.a
)

# add glfw3.dll to directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/lib/glfw3.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>)