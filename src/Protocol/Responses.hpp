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

    struct OpenNoteResponse {
        Operation op;
        uint8_t status;        // 0 = успех, 1 = ошибка
        uint32_t note_id;
        std::string title; // пока что пусть будет чтобы извещать других пользователей об изменении названия заметки
        std::string text;
    };

    struct UpdateTextResponse {
        Operation op;
        uint8_t status;
        uint32_t note_id;
    };
}
