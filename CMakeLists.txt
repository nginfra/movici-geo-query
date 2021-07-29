cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(boost_geo_query)


set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.71.0 REQUIRED
    COMPONENTS
    ) 

include_directories(${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/tests/c)
# link_libraries(${Boost_LIBRARIES})


if (WIN32)
  add_definitions( -DBOOST_ALL_NO_LIB )
  add_definitions( -DBOOST_ALL_DYN_LINK )
endif()

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/python_geo_query.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/c/boost_geo_query_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/c/python_geo_query_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/c/test_main.cpp
)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++17 -g -Wall -DNOPYBIND")
# set(CMAKE_CXX_FLAGS  "-O2 -Wall -DNOPYBIND")

add_executable(test ${SOURCES})



# -- OPTIONALLY build with pybind11 : remove -DNOPYBIND from CXX_FLAGS --

# set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall")
# find_package(pybind11 CONFIG)
# pybind11_add_module(test ${SOURCES})