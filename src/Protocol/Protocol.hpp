#pragma once

#include <vector>
#include <optional>

#include "Requests.hpp"
#include "Responses.hpp"

#include "Operation.hpp"

namespace Protocol {

    // for all requests and responses
    Operation decodeOperation(const std::vector<uint8_t>& buffer);

    // AUTH
    std::vector<uint8_t> encodeAuthRequest(const AuthRequest& req);
    std::vector<uint8_t> encodeAuthResponse(const AuthResponse& resp);
    std::optional<AuthResponse> decodeAuthResponse(const std::vector<uint8_t>& buffer);
    std::optional<AuthRequest> decodeAuthRequest(const std::vector<uint8_t>& buffer);

    // REGISTRATION
    std::vector<uint8_t> encodeRegistrationRequest(const RegistrationRequest& req);
    std::vector<uint8_t> encodeRegistrationResponse(const RegistrationResponse& resp);
    std::optional<RegistrationRequest> decodeRegistrationRequest(const std::vector<uint8_t>& buffer);
    std::optional<RegistrationResponse> decodeRegistrationResponse(const std::vector<uint8_t>& buffer);

    // SYNC
    std::vector<uint8_t> encodeSyncRequest(const SyncNoteRequest& req);
    std::vector<uint8_t> encodeSyncResponse(const SyncNoteResponse& resp);
    std::optional<SyncNoteRequest> decodeSyncRequest(const std::vector<uint8_t>& buffer);
    std::optional<SyncNoteResponse> decodeSyncResponse(const std::vector<uint8_t>& buffer);

    // GET_NOTES
    std::vector<uint8_t> encodeGetNotesRequest(const GetNotesRequest& req);
    std::vector<uint8_t> encodeGetNotesResponse(const GetNotesResponse& response);
    std::optional<GetNotesRequest> decodeGetNotesRequest(const std::vector<uint8_t>& buffer);
    std::optional<GetNotesResponse> decodeGetNotesResponse(const std::vector<uint8_t>& buffer);

    // CREATE_NOTE
    std::vector<uint8_t> encodeCreateNoteRequest(const CreateNoteRequest& req);
    std::vector<uint8_t> encodeCreateNoteResponse(const CreateNoteResponse& resp);
    std::optional<CreateNoteRequest> decodeCreateNoteRequest(const std::vector<uint8_t>& buffer);
    std::optional<CreateNoteResponse> decodeCreateNoteResponse(const std::vector<uint8_t>& buffer);

    // OPEN_NOTE
    std::vector<uint8_t> encodeOpenNoteRequest(const OpenNoteRequest& req);
    std::vector<uint8_t> encodeOpenNoteResponse(const OpenNoteResponse& resp);
    std::optional<OpenNoteRequest> decodeOpenNoteRequest(const std::vector<uint8_t>& buffer);
    std::optional<OpenNoteResponse> decodeOpenNoteResponse(const std::vector<uint8_t>& buffer);

    // UPDATE_TEXT
    std::vector<uint8_t> encodeUpdateTextRequest(const UpdateTextRequest& req);
    std::vector<uint8_t> encodeUpdateTextResponse(const UpdateTextResponse& resp);
    std::optional<UpdateTextRequest> decodeUpdateTextRequest(const std::vector<uint8_t>& buffer);
    std::optional<UpdateTextResponse> decodeUpdateTextResponse(const std::vector<uint8_t>& buffer);

    // SHARE_NOTE
    std::vector<uint8_t> encodeShareNoteRequest(const ShareNoteRequest& req);
    std::vector<uint8_t> encodeShareNoteResponse(const ShareNoteResponse& resp);
    std::optional<ShareNoteRequest> decodeShareNoteRequest(const std::vector<uint8_t>& buffer);
    std::optional<ShareNoteResponse> decodeShareNoteResponse(const std::vector<uint8_t>& buffer);

    // SHARE_NOTE_NOTIFY
    std::vector<uint8_t> encodeShareNoteNotifyRequest(const ShareNoteNotifyRequest& req);
    std::vector<uint8_t> encodeShareNoteNotifyResponse(const ShareNoteNotifyResponse& resp);
    std::optional<ShareNoteNotifyRequest> decodeShareNoteNotifyRequest(const std::vector<uint8_t>& buffer);
    std::optional<ShareNoteNotifyResponse> decodeShareNoteNotifyResponse(const std::vector<uint8_t>& buffer);
};