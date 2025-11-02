#pragma once

#include <vector>
#include <optional>

#include <json/json.hpp>

class Protocol {
public:
    static std::vector<uint8_t> encode(const nlohmann::json& msg);
    static std::optional<nlohmann::json> decode(const char* data, size_t size);
};