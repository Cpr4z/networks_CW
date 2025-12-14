#pragma once

#include "Operation.hpp"

#include<string>

namespace Protocol {
    struct AuthRequest {
        std::string login;
        std::string password;
    };

    struct AuthResponse {
        Operation op;
        uint8_t status;
        uint32_t user_id;
    };

    struct RegistrationRequest {
        std::string login;
        std::string password;
    };

    struct RegistrationResponse {
        Operation op;
        uint8_t status;
        uint32_t user_id;
    };

    struct CreateNoteRequest {
//        Operation op;
    };


    struct CreateNoteResponse {
//        Operation op;
    };

    struct SyncNoteRequest {
//        Operation op;
    };

    struct SyncNoteResponse {
//        Operation op;
    };
}