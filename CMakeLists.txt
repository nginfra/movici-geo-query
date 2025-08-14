cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(boost_geo_query LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Handle modern CMake policy for Boost
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

# Check if BOOST_DIR is set (e.g., in CI)
if(DEFINED ENV{BOOST_DIR})
    set(Boost_INCLUDE_DIRS "$ENV{BOOST_DIR}")
    set(Boost_FOUND TRUE)
    message(STATUS "Using Boost from BOOST_DIR: ${Boost_INCLUDE_DIRS}")
else()
    # Try to find Boost locally first
    find_package(Boost 1.79.0 QUIET COMPONENTS)
endif()

if(NOT Boost_FOUND)
    message(STATUS "Boost not found locally, downloading via FetchContent...")
    include(FetchContent)
    
    # Use appropriate archive format for the platform
    if(WIN32)
        FetchContent_Declare(
            Boost
            URL https://archives.boost.io/release/1.79.0/source/boost_1_79_0.zip
            URL_HASH SHA256=3634f9a85759311f321e587eace21799c0d0c946ff933e477a2f98885c54bbff
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
    else()
        FetchContent_Declare(
            Boost
            URL https://archives.boost.io/release/1.79.0/source/boost_1_79_0.tar.gz
            URL_HASH SHA256=273f1be93238a068aba4f9735a4a2b003019af067b9c183ed227780b8f36062c
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
    endif()
    
    set(BOOST_INCLUDE_LIBRARIES geometry)
    set(BOOST_ENABLE_CMAKE ON)
    FetchContent_MakeAvailable(Boost)
    
    # Create interface target for compatibility
    add_library(boost_headers INTERFACE)
    target_include_directories(boost_headers INTERFACE 
        ${boost_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src 
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/c
    )
else()
    # Use system Boost
    add_library(boost_headers INTERFACE)
    target_include_directories(boost_headers INTERFACE 
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src 
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/c
    )
endif()

# Windows-specific definitions
if(WIN32)
    add_definitions(-DBOOST_ALL_NO_LIB)
    add_definitions(-DBOOST_ALL_DYN_LINK)
endif()

# Test sources
set(TEST_SOURCES
    src/python_geo_query.cpp
    tests/c/boost_geo_query_test.cpp
    tests/c/python_geo_query_test.cpp
    tests/c/test_main.cpp
)

# Create test executable
add_executable(test ${TEST_SOURCES})

# Set compiler flags
target_compile_definitions(test PRIVATE -DNOPYBIND)
target_compile_options(test PRIVATE 
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Link with Boost
target_link_libraries(test PRIVATE boost_headers)

# Enable debug info in debug builds
target_compile_options(test PRIVATE $<$<CONFIG:Debug>:-g>)



# -- OPTIONALLY build with pybind11 : remove -DNOPYBIND from CXX_FLAGS --

# set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall")
# find_package(pybind11 CONFIG)
# pybind11_add_module(test ${SOURCES})