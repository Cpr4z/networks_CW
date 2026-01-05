cmake_minimum_required(VERSION 3.28)
project(client)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Qml Network QuickControls2)

set(HEADERS
    src/Client.hpp)

set(SOURCES
    src/Client.cpp)

set(QT
    qml.qrc
    src/AuthManager.hpp
    src/AuthManager.cpp
    src/NotesManager.hpp
    src/NotesManager.cpp
    src/NotesModel.hpp
    src/NotesModel.cpp
    )

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${QT} main_client.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Quick
        Qt6::Qml
        Qt6::Gui
        Qt6::Network
        Qt6::QuickControls2
        protocol)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/Protocol
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/json
        ${CMAKE_SOURCE_DIR}/utils)