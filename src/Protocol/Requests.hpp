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

    struct OpenNoteRequest {
        uint32_t note_id;
        uint32_t user_id;
        uint32_t version;
    };

    struct UpdateTextRequest {
        uint32_t user_id;
        uint32_t note_id;
        uint32_t version;
        std::string text;
    };

    struct ShareNoteRequest {
        uint32_t user_id;
        uint32_t note_id;
    };

    struct ShareNoteNotifyRequest {
        Operation op;
        uint32_t note_id;
        uint32_t owner_id;
        uint32_t version;
        std::string note_title;
//        std::string note_text;
    };
}