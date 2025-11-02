add_library(protocol STATIC
        Document.cpp
        User.cpp
        Protocol.cpp
)

target_compile_features(protocol PUBLIC cxx_std_23)

target_include_directories(protocol PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/json
        ${CMAKE_SOURCE_DIR}/utils)