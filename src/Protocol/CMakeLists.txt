add_library(protocol STATIC
        Operation.hpp
        Requests.hpp
        Responses.hpp
        Note.cpp
        User.cpp
        Protocol.cpp
        Encryptor.cpp
)

find_package(OpenSSL REQUIRED)

target_include_directories(protocol PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(protocol PRIVATE ${OPENSSL_LIBRARIES})

target_compile_features(protocol PUBLIC cxx_std_23)

target_include_directories(protocol PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/json
        ${CMAKE_SOURCE_DIR}/utils
)