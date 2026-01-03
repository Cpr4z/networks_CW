#pragma once

#include <map>

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
        std::map<uint32_t, std::tuple<std::string, bool, uint32_t, uint32_t>> notes;
    };

    struct CreateNoteResponse {
        Operation op;
        uint8_t status;
        uint32_t note_id;
        uint32_t version;
        std::string note_title;
    };

    struct SyncNoteResponse {
        Operation op;
        uint32_t version;
        std::string text;
    };

    struct OpenNoteResponse {
        Operation op;
        uint8_t status;
        uint32_t note_id;
        uint32_t version;
        std::string title;
        std::string text;
    };

    struct UpdateTextResponse {
        Operation op;
        uint8_t status;
        uint32_t note_id;
        uint32_t version;
    };

    struct OwnerApproveMergeResponse {
        Operation op;
        uint32_t note_id;
        uint32_t sender_id;
        uint32_t new_version;
        uint8_t status;
        std::string approved_text;
    };
}
