#pragma once

#include <string>

// insert
// delete
// update
// selection_change
// sync_request
// sync_response
// user_add
// user_delete
// ack
// nack

// cursor_move ??

namespace Protocol {
    enum class Operation: uint16_t {
        AUTH = 0x0001,
        REGISTRATION = 0x0002,
        GET_NOTES = 0x0003,
        SYNC = 0x0004,
        CREATE_NOTE = 0x0005,
        OPEN_NOTE = 0x0006,
        UPDATE_TEXT = 0x0007,
        INSERT,
        DELETE,
        UPDATE,
        SELECTION_CHANGE,
        DELETE_USER,
        ACK,
        NACK
    };
}
