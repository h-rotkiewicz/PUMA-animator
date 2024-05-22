# see https://alexreinking.com/blog/how-to-use-cmake-without-the-agonizing-pain-part-1.html
include(ExternalProject)
cmake_minimum_required(VERSION 3.29)
set(CMAKE_CXX_STANDARD 20)
option(WIN32 "Windows build" OFF)
project(PUMA)

find_package( OpenGL REQUIRED )

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
set( GLAD_GL "" )
add_compile_definitions(SHADERS_DIR="${CMAKE_SOURCE_DIR}/src/shaders")
add_compile_definitions(RESOURCES_DIR="${CMAKE_SOURCE_DIR}/src/resources")

# # option( USE-GLFW-GLAD "Use GLAD from GLFW" ON )
#
# if(USE-GLFW-GLAD)
#   include_directories("${GLFW_SOURCE_DIR}/deps")
#   set( GLAD_GL "${GLFW_SOURCE_DIR}/deps/glad/gl.h" )
# endif()
#


if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/includes/glfw)
add_subdirectory(${CMAKE_SOURCE_DIR}/includes/glm)

if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  find_package(Catch2 3 REQUIRED)
  add_subdirectory(Tests)
endif()



file(GLOB SOURCES "src/*.cpp" "src/*.c")

include_directories(${CMAKE_SOURCE_DIR}/includes)
include_directories(${CMAKE_SOURCE_DIR}/includes/glm/)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} glfw glm)
if( MSVC )
    if(${CMAKE_VERSION} VERSION_LESS "3.6.0")
        message( "\n\t[ WARNING ]\n\n\tCMake version lower than 3.6.\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'GLFW-CMake-starter' as StartUp Project in Visual Studio.\n" )
    else()
        set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GLFW-CMake-starter )
    endif()
endif()
