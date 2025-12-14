cmake_minimum_required(VERSION 3.29)
project(server)

set(CMAKE_CXX_STANDARD 23)

set(HEADERS
        Server.hpp
        AcceptAwaiter.hpp
        ReadAwaiter.hpp
        WriteAwaiter.hpp
        Reactor.hpp
        Task.hpp
        SyncServer.hpp
#        User.hpp
#        Document.hpp
        Repository.hpp
        )

set(SOURCES
        Server.cpp
        AcceptAwaiter.cpp
        ReadAwaiter.cpp
        WriteAwaiter.cpp
        Reactor.cpp
        Task.cpp
        SyncServer.cpp
#        Document.cpp
#        User.cpp
        Repository.cpp
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main_server.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
        protocol
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/Protocol
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/json
        ${CMAKE_SOURCE_DIR}/utils
        )