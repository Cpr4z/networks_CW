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

    struct GetNotesRequest {
//        Operation op;
        uint32_t user_id;
    };

    struct GetNotesResponse {
        Operation op;
        // ??
    };

    struct CreateNoteRequest {
        uint32_t user_id;
        std::string note_title;
//        Operation op;
    };

    struct CreateNoteResponse {
        Operation op;
        uint8_t status;
        uint32_t note_id;
    };

    struct SyncNoteRequest {
//        Operation op;
    };

    struct SyncNoteResponse {
        Operation op;
    };
}