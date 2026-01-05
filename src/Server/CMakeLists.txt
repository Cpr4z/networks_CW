cmake_minimum_required(VERSION 3.28)
project(server)

set(CMAKE_CXX_STANDARD 23)

set(HEADERS
        Server.hpp
        Repository.hpp
        )

set(SOURCES
        Server.cpp
        Repository.cpp
)

find_package(OpenSSL REQUIRED)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main_server.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OPENSSL_LIBRARIES}
        protocol
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${OPENSSL_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/Protocol
        ${CMAKE_SOURCE_DIR}/include
        )