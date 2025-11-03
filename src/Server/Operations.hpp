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
enum class Operations {
    INSERT,
    DELETE,
    UPDATE,
    SELECTION_CHANGE,
    SYNC_REQUEST,
    SYNC_RESPONSE,
    ADD_USER,
    DELETE_USER,
    ACK,
    NACK
};
