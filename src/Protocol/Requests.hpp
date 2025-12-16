#pragma once

#include "Operation.hpp"

#include<string>

namespace Protocol {
    struct AuthRequest {
        std::string login;
        std::string password;
    };

    struct RegistrationRequest {
        std::string login;
        std::string password;
    };

    struct GetNotesRequest {
//        Operation op;
        uint32_t user_id;
    };

    struct CreateNoteRequest {
        uint32_t user_id;
        std::string note_title;
//        Operation op;
    };

    struct SyncNoteRequest {
//        Operation op;
    };
}