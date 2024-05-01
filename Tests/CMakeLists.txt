# Download Catch2 from GitHub and make it available to be included
find_package(Catch2 3 REQUIRED)

# file(GLOB_RECURSE tested_files ${CMAKE_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE tests ${CMAKE_SOURCE_DIR}/Tests/*.cpp )

file(GLOB_RECURSE includes ${CMAKE_SOURCE_DIR}/src/*.h)

file(GLOB_RECURSE includes_impl ${CMAKE_SOURCE_DIR}/src/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(test ${tests} )
target_link_libraries(test PRIVATE ${includes_impl})

# Link Catch2 to the test executable
target_link_libraries(test PRIVATE Catch2::Catch2WithMain)

