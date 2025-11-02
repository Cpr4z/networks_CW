cmake_minimum_required(VERSION 3.29)
project(client)

set(CMAKE_CXX_STANDARD 23)

set(HEADERS
    Client.hpp)

set(SOURCES
    Client.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main_client.cpp)

target_link_libraries(${PROJECT_NAME} protocol)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/Protocol
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/json
        ${CMAKE_SOURCE_DIR}/utils)