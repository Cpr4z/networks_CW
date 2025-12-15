#pragma once

#include <vector>
#include <optional>

#include "Requests.hpp"

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
};