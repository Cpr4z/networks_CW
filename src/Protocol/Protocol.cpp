#include "Protocol.hpp"

std::vector<uint8_t> Protocol::encode(const nlohmann::json& msg) {
    std::string json = msg.dump();
    uint32_t len = htonl(static_cast<uint32_t>(json.size()));
    std::vector<uint8_t> buf(sizeof(len) + json.size());
    memcpy(buf.data(), &len, sizeof(len));
    memcpy(buf.data() + sizeof(len), json.data(), json.size());
    return buf;
}

std::optional<nlohmann::json> Protocol::decode(const char *data, size_t size) {
    if (size < sizeof(uint32_t))
        return std::nullopt;

    uint32_t len = 0;
    memcpy(&len, data, sizeof(len));
    len = ntohl(len);

    if (size < sizeof(uint32_t) + len)
        return std::nullopt;

    try {
        std::string json_str(data + sizeof(uint32_t), len);
        return nlohmann::json::parse(json_str);
    } catch (...) {
        return std::nullopt;
    }
}