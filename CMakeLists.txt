cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(boost_geo_query LANGUAGES CXX)

# Boost version configuration
set(BOOST_VERSION "1.89.0")
string(REPLACE "." "_" BOOST_VERSION_UNDERSCORE "${BOOST_VERSION}")
set(BOOST_ZIP_HASH "SHA256=77bee48e32cabab96a3fd2589ec3ab9a17798d330220fdd8bde6ff5611b4ccde")
set(BOOST_TAR_HASH "SHA256=9de758db755e8330a01d995b0a24d09798048400ac25c03fc5ea9be364b13c93")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Handle modern CMake policy for Boost
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

if(DEFINED ENV{BOOST_DIR})
    if (NOT EXISTS "$ENV{BOOST_DIR}/boost/version.hpp")
        message(FATAL_ERROR "BOOST_DIR is set to '$ENV{BOOST_DIR}' but does not contain Boost headers.")
    endif()

    set(Boost_INCLUDE_DIRS "$ENV{BOOST_DIR}")
    set(Boost_FOUND TRUE)
    message(STATUS "Using Boost from BOOST_DIR: ${Boost_INCLUDE_DIRS}")
else()
    # Try to find Boost locally first
    find_package(Boost ${BOOST_VERSION} QUIET COMPONENTS)
endif()

if(NOT Boost_FOUND)
    message(STATUS "Boost not found locally, downloading via FetchContent...")
    include(FetchContent)

    # Use appropriate archive format for the platform
    if(WIN32)
        FetchContent_Declare(
            Boost
            URL https://archives.boost.io/release/${BOOST_VERSION}/source/boost_${BOOST_VERSION_UNDERSCORE}.zip
            URL_HASH ${BOOST_ZIP_HASH}
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
    else()
        FetchContent_Declare(
            Boost
            URL https://archives.boost.io/release/${BOOST_VERSION}/source/boost_${BOOST_VERSION_UNDERSCORE}.tar.gz
            URL_HASH ${BOOST_TAR_HASH}
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

# Options to control what gets built
option(BUILD_PYTHON_MODULE "Build Python module" ON)
option(BUILD_TESTS "Build C++ tests" OFF)

# Build C++ tests only if requested
if(BUILD_TESTS)
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
endif()



# -- Build Python module with pybind11 --

if(BUILD_PYTHON_MODULE)
    # Find Python and pybind11 for building the Python extension
    find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
    find_package(pybind11 CONFIG REQUIRED)

    # Python module sources
    set(PYTHON_MODULE_SOURCES
        src/_movici_geo_query.cpp
        src/python_geo_query.cpp
    )

    # Create Python module
    pybind11_add_module(_movici_geo_query ${PYTHON_MODULE_SOURCES})

    # Link with Boost headers
    target_link_libraries(_movici_geo_query PRIVATE boost_headers)

    # Set compiler options for the Python module
    target_compile_options(_movici_geo_query PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )

    # Enable optimization in release builds
    target_compile_options(_movici_geo_query PRIVATE $<$<CONFIG:Release>:-O3>)

    # Enable debug info in debug builds
    target_compile_options(_movici_geo_query PRIVATE $<$<CONFIG:Debug>:-g>)

    # Install the Python module - Adding the install directive ensures the compiled .so file is
    # properly packaged into the wheel.
    install(TARGETS _movici_geo_query LIBRARY DESTINATION .)
endif()
