cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(boost_geo_query
    VERSION 1.2.0
    DESCRIPTION "Geospatial queries powered by Boost Geometry"
    LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTING "Build the testing tree" ON)
option(USE_SYSTEM_BOOST "Use system-installed Boost instead of FetchContent" ON)

# Force system Boost when we know it's available (e.g., in CI with cache)
if(EXISTS "$ENV{HOME}/boost_cache")
    message(STATUS "Boost cache found - but system boost may not be properly installed")
    # Don't force system boost, let it fall back to FetchContent
    # The cache step doesn't actually install boost properly
elseif(EXISTS "/usr/include/boost" AND USE_SYSTEM_BOOST)
    message(STATUS "System Boost headers found at /usr/include/boost")
    set(Boost_FOUND TRUE)
    set(Boost_INCLUDE_DIRS "/usr/include")
endif()

# Try to find Boost first - be more aggressive about finding system Boost
if(USE_SYSTEM_BOOST)
    # Check common system locations first
    if(EXISTS "/usr/include/boost")
        message(STATUS "Found system Boost headers at /usr/include/boost")
        set(Boost_FOUND TRUE)
        set(Boost_INCLUDE_DIRS "/usr/include")
    elseif(EXISTS "/usr/local/include/boost")
        message(STATUS "Found system Boost headers at /usr/local/include/boost")
        set(Boost_FOUND TRUE)
        set(Boost_INCLUDE_DIRS "/usr/local/include")
    else()
        # Try standard CMake find
        find_package(Boost 1.71.0 QUIET)
        if(NOT Boost_FOUND)
            find_package(Boost QUIET)
        endif()
    endif()
endif()

# If Boost not found, use FetchContent
if(NOT Boost_FOUND)
    message(STATUS "Boost not found on system, downloading via FetchContent...")
    include(FetchContent)

    # Download specific Boost libraries we need
    FetchContent_Declare(
        Boost
        URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.gz
        URL_HASH SHA256=4d27e9efed0f6f152dc28db6430b9d3dfb40c0345da7342eaa5a987dde57bd95
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_MakeAvailable(Boost)

    # Set Boost variables for compatibility
    set(BOOST_ROOT ${boost_SOURCE_DIR})

    # Simple solution: collect all libs/*/include directories
    file(GLOB BOOST_LIB_INCLUDES "${boost_SOURCE_DIR}/libs/*/include")
    set(Boost_INCLUDE_DIRS "${boost_SOURCE_DIR}")
    foreach(BOOST_LIB_INCLUDE ${BOOST_LIB_INCLUDES})
        list(APPEND Boost_INCLUDE_DIRS ${BOOST_LIB_INCLUDE})
    endforeach()

    # Special handling for numeric conversion headers
    # In Boost 1.84, they are in libs/numeric/conversion/include
    if(EXISTS "${boost_SOURCE_DIR}/libs/numeric/conversion/include")
        list(APPEND Boost_INCLUDE_DIRS "${boost_SOURCE_DIR}/libs/numeric/conversion/include")
    endif()

    # Debug: print found paths (uncomment for debugging)
    # message(STATUS "Boost source dir: ${boost_SOURCE_DIR}")
    # string(REPLACE ";" "\n  " BOOST_DIRS_FORMATTED "${Boost_INCLUDE_DIRS}")
    # message(STATUS "Found Boost include dirs:\n  ${BOOST_DIRS_FORMATTED}")
endif()

# Platform-specific definitions
if(WIN32)
    add_compile_definitions(BOOST_ALL_NO_LIB BOOST_ALL_DYN_LINK)
endif()

# Create test executable only if building tests
if(BUILD_TESTING AND CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    enable_testing()

    # Define sources
    set(TEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/python_geo_query.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/c/boost_geo_query_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/c/python_geo_query_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/c/test_main.cpp
    )

    # Create test executable
    add_executable(geo_query_test ${TEST_SOURCES})

    # Modern CMake target-based commands
    target_include_directories(geo_query_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/c
    )

    # Always use include directories approach for FetchContent Boost
    target_include_directories(geo_query_test PRIVATE ${Boost_INCLUDE_DIRS})

    target_compile_features(geo_query_test PRIVATE cxx_std_17)

    target_compile_options(geo_query_test PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra>
        $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Debug>>:-g -O0>
        $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Release>>:-O3 -DNDEBUG>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )

    target_compile_definitions(geo_query_test PRIVATE NOPYBIND)

    # Add test
    add_test(NAME boost_geo_query_tests COMMAND geo_query_test)
endif()

# Export compile commands for development tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Installation rules (optional)
include(GNUInstallDirs)
install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/boost_geo_query
    FILES_MATCHING PATTERN "*.hpp")
