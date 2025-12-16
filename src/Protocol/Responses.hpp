#pragma once

#include "Operation.hpp"

namespace Protocol {
    struct AuthResponse {
        Operation op;
        uint8_t status;
        uint32_t user_id;
    };

    struct RegistrationResponse {
        Operation op;
        uint8_t status;
        uint32_t user_id;
    };

    struct GetNotesResponse {
        Operation op;
        // ??
    };

    struct CreateNoteResponse {
        Operation op;
        uint8_t status;
        uint32_t note_id;
        std::string note_title;
    };

    struct SyncNoteResponse {
        Operation op;
    };
}
