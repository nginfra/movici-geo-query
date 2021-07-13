cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(boost_geo_query)


set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.76.0 REQUIRED 
    COMPONENTS
    ) 

include_directories(${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}) 
# link_libraries(${Boost_LIBRARIES})

if (WIN32)
  add_definitions( -DBOOST_ALL_NO_LIB )
  add_definitions( -DBOOST_ALL_DYN_LINK )
endif()

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/python_geo_query.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unitTest/boost_geo_query_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unitTest/python_geo_query_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unitTest/test_main.cpp
)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall")
# set(CMAKE_CXX_FLAGS  "-O2 -Wall")
# add_library(test ${SOURCES})
add_executable(test ${SOURCES})