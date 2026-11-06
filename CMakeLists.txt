cmake_minimum_required(VERSION 3.30.5)
project(opengl_test
    VERSION 0.0.1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# fix for AVX spills
if (MINGW)
    add_compile_options(-mstackrealign)
endif()

# turn off glfw building as a shared lib
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "") # forces static rebuilding

# turn off fastnoise building for gui tool
set(FASTNOISE2_NOISETOOL OFF CACHE BOOL "" FORCE)
set(FASTNOISE2_TESTS OFF CACHE BOOL "" FORCE)

# get glfw from github repo
include(FetchContent)
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(glfw)
# get fastnoise2
FetchContent_Declare(FastNoise2
    GIT_REPOSITORY https://github.com/Auburn/FastNoise2.git
    GIT_TAG v1.1.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(FastNoise2)


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
        src/gl/Framebuffer.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}-${PROJECT_VERSION}"
)

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
        glfw
        FastNoise2
)