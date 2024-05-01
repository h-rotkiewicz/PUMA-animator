# see https://alexreinking.com/blog/how-to-use-cmake-without-the-agonizing-pain-part-1.html
cmake_minimum_required(VERSION 3.29)
set(CMAKE_CXX_STANDARD 20)
project(PUMA)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

include(FetchContent)
FetchContent_Declare(
    DiligentCore
    GIT_REPOSITORY https://github.com/DiligentGraphics/DiligentCore.git
    SOURCE_DIR _deps/DiligentCore
)
FetchContent_Declare(
    DiligentTools
    GIT_REPOSITORY https://github.com/DiligentGraphics/DiligentTools.git
    SOURCE_DIR _deps/DiligentTools
)
FetchContent_Declare(
    DiligentFX
    GIT_REPOSITORY https://github.com/DiligentGraphics/DiligentFX.git
    SOURCE_DIR _deps/DiligentFX
)
FetchContent_MakeAvailable(DiligentCore DiligentTools DiligentFX)



if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  find_package(Catch2 3 REQUIRED)
  add_subdirectory(Tests)
endif()

file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
PRIVATE
    ${diligentcore_SOURCE_DIR}
    ${diligenttools_SOURCE_DIR}
    ${diligentfx_SOURCE_DIR}
)
target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    Diligent-BuildSettings
    # Diligent-GraphicsEngineD3D11-shared
    # Diligent-GraphicsEngineD3D12-shared
    Diligent-GraphicsEngineOpenGL-shared
    Diligent-GraphicsEngineVk-shared
    DiligentFX
)
# copy_required_dlls(${PROJECT_NAME})
