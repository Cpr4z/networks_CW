#pragma once

#include <vector>
#include <optional>

#include "Requests.hpp"

#include "Operation.hpp"

#include <json/json.hpp>

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
};