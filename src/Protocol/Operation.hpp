#pragma once

#include <string>

namespace Protocol {
    enum class Operation: uint16_t {
        AUTH = 0x0001,
        REGISTRATION = 0x0002,
        GET_NOTES = 0x0003,
        SYNC = 0x0004,
        CREATE_NOTE = 0x0005,
        OPEN_NOTE = 0x0006,
        UPDATE_TEXT = 0x0007,
        SHARE_NOTE = 0x0008,
        SHARE_NOTE_NOTIFY = 0x0009,
        APPROVE_MERGE = 0x0010,
        OWNER_APPROVE_MERGE = 0x0011,
    };
}
