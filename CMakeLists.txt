
cmake_minimum_required (VERSION 3.1)

project(digidesign_c24_osc_query LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

set(
    OSSIA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libossia
    CACHE PATH ""
)

set(
    C24_DIR ${CMAKE_CURRENT_SOURCE_DIR}/avid-c24-driver 
    CACHE PATH ""
)

add_subdirectory(${OSSIA_DIR} OSSIA)
add_subdirectory(${C24_DIR} c24_driver)

set(C24_OSC_QUERY_SRC
	src/main.cpp
	src/c24_protocol.hpp
	src/c24_protocol.cpp
	src/c24_parameter.hpp
	src/c24_parameter.cpp
	src/device_parameter.hpp
	src/device_parameter.cpp
)

add_executable(c24_osc_query ${C24_OSC_QUERY_SRC})	
target_include_directories(c24_osc_query PRIVATE ${CMAKE_SOURCE_DIR}/../device_parameter)
target_link_libraries(c24_osc_query PRIVATE c24_driver ossia)

