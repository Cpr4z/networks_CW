#include "Protocol.hpp"

#include <cstdint>
#include <cstring>

namespace Protocol {
    Operation decodeOperation(const std::vector<uint8_t>& buffer) {
        uint16_t opCode;
        std::memcpy(&opCode, buffer.data(), sizeof(uint16_t));
        return static_cast<Operation>(opCode);
    }

    std::vector<uint8_t> encodeAuthRequest(const AuthRequest& req) {
        std::string login = req.login;
        std::string password = req.password;

        if (!login.empty() && login.back() == '\0') {
            login.pop_back();
        }
        if (!password.empty() && password.back() == '\0') {
            password.pop_back();
        }

        auto loginLen = static_cast<uint16_t>(login.size());
        auto passwordLen = static_cast<uint16_t>(password.size());

        size_t totalSize = 2;
        totalSize += 2;
        totalSize += 2;
        totalSize += loginLen;
        totalSize += passwordLen;

        std::vector<uint8_t> buffer(totalSize);
        uint8_t* ptr = buffer.data();

        auto opCode = static_cast<uint16_t>(Protocol::Operation::AUTH);
        std::memcpy(ptr, &opCode, sizeof(opCode));
        ptr += sizeof(opCode);

        std::memcpy(ptr, &loginLen, sizeof(loginLen));
        ptr += sizeof(loginLen);

        std::memcpy(ptr, &passwordLen, sizeof(passwordLen));
        ptr += sizeof(passwordLen);

        std::memcpy(ptr, login.data(), loginLen);
        ptr += loginLen;

        std::memcpy(ptr, password.data(), passwordLen);

        return buffer;
    }

    std::vector<uint8_t> encodeAuthResponse(const AuthResponse& resp) {
        std::vector<uint8_t> buffer(7);
        uint8_t* ptr = buffer.data();

        uint16_t op = static_cast<uint16_t>(resp.op);
        std::memcpy(ptr, &op, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        std::memcpy(ptr, &resp.status, sizeof(uint8_t));
        ptr += sizeof(uint8_t);

        std::memcpy(ptr, &resp.user_id, sizeof(uint32_t));

        return buffer;
    }

    std::optional<AuthResponse> decodeAuthResponse(const std::vector<uint8_t>& buffer) {
        constexpr size_t MIN_SIZE = 7;
        if (buffer.size() < MIN_SIZE) {
            return std::nullopt;
        }

        AuthResponse response{};
        const uint8_t* ptr = buffer.data();

        std::memcpy(&response.op, ptr, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        std::memcpy(&response.status, ptr, sizeof(uint8_t));
        ptr += sizeof(uint8_t);

        std::memcpy(&response.user_id, ptr, sizeof(uint32_t));

        return response;
    }

    std::optional<AuthRequest> decodeAuthRequest(const std::vector<uint8_t>& buffer) {
        if (buffer.size() < 6) {
            return std::nullopt;
        }

        AuthRequest request;
        const uint8_t* ptr = buffer.data();

        uint16_t opCode;
        std::memcpy(&opCode, ptr, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        uint16_t loginLen;
        std::memcpy(&loginLen, ptr, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        uint16_t passwordLen;
        std::memcpy(&passwordLen, ptr, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        size_t requiredSize = 6 + loginLen + passwordLen;
        if (buffer.size() < requiredSize) {
            return std::nullopt;
        }

        if (loginLen > 0) {
            request.login.assign(reinterpret_cast<const char*>(ptr), loginLen);
            ptr += loginLen;
        } else {
            request.login = "";
        }

        if (passwordLen > 0) {
            request.password.assign(reinterpret_cast<const char*>(ptr), passwordLen);
        } else {
            request.password = "";
        }

        return request;
    }

    std::vector<uint8_t> encodeRegistrationRequest(const RegistrationRequest& req) {
        std::string login = req.login;
        std::string password = req.password;

        if (!login.empty() && login.back() == '\0') {
            login.pop_back();
        }
        if (!password.empty() && password.back() == '\0') {
            password.pop_back();
        }

        auto loginLen = static_cast<uint16_t>(login.size());
        auto passwordLen = static_cast<uint16_t>(password.size());

        size_t totalSize = 2;
        totalSize += 2;
        totalSize += 2;
        totalSize += loginLen;
        totalSize += passwordLen;

        std::vector<uint8_t> buffer(totalSize);
        uint8_t* ptr = buffer.data();

        auto opCode = static_cast<uint16_t>(Protocol::Operation::REGISTRATION);
        std::memcpy(ptr, &opCode, sizeof(opCode));
        ptr += sizeof(opCode);

        std::memcpy(ptr, &loginLen, sizeof(loginLen));
        ptr += sizeof(loginLen);

        std::memcpy(ptr, &passwordLen, sizeof(passwordLen));
        ptr += sizeof(passwordLen);

        std::memcpy(ptr, login.data(), loginLen);
        ptr += loginLen;

        std::memcpy(ptr, password.data(), passwordLen);

        return buffer;
    }

    std::vector<uint8_t> encodeRegistrationResponse(const RegistrationResponse& resp) {
        std::vector<uint8_t> buffer(7);
        uint8_t* ptr = buffer.data();
        auto op = static_cast<uint16_t>(resp.op);
        std::memcpy(ptr, &op, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        std::memcpy(ptr, &resp.status, sizeof(uint8_t));
        ptr += sizeof(uint8_t);

        std::memcpy(ptr, &resp.user_id, sizeof(uint32_t));

        return buffer;
    }

    std::optional<RegistrationRequest> decodeRegistrationRequest(const std::vector<uint8_t>& buffer) {
        const uint8_t* ptr = buffer.data();
        uint16_t opCode = 0;
        std::memcpy(&opCode, ptr, 2);
        ptr += 2;

        if (static_cast<Protocol::Operation>(opCode) != Protocol::Operation::REGISTRATION) {
            return std::nullopt;
        }

        uint16_t loginLen = 0;
        std::memcpy(&loginLen, ptr, sizeof(loginLen));
        ptr += sizeof(loginLen);

        uint16_t passwordLen = 0;
        std::memcpy(&passwordLen, ptr, sizeof(passwordLen));
        ptr += sizeof(passwordLen);

        if (buffer.size() < 6 + loginLen + passwordLen) {
            return std::nullopt;
        }

        std::string login(reinterpret_cast<const char*>(ptr), loginLen);
        ptr += loginLen;

        std::string password(reinterpret_cast<const char*>(ptr), passwordLen);

        return RegistrationRequest{
                .login = std::move(login),
                .password = std::move(password)
        };
    }

    std::optional<RegistrationResponse> decodeRegistrationResponse(const std::vector<uint8_t>& buffer) {
        RegistrationResponse response;
        const uint8_t* ptr = buffer.data();

        std::memcpy(&response.op, ptr, sizeof(response.op));
        ptr += sizeof(response.op);

        std::memcpy(&response.status, ptr, sizeof(response.status));
        ptr += sizeof(response.status);

        response.user_id = 0;
        if (response.status == 0) {
            std::memcpy(&response.user_id, ptr, sizeof(response.user_id));
        }
        return response;
    }

    std::vector<uint8_t> encodeSyncRequest(const SyncNoteRequest& req) {
        const uint32_t base_text_size = static_cast<uint32_t>(req.base_version.size());
        const uint32_t text_size = static_cast<uint32_t>(req.local_version.size());

        std::vector<uint8_t> buffer(2 + 4 + 4 + 4 + 4 + base_text_size + text_size);

        uint8_t* ptr = buffer.data();

        const uint16_t op = static_cast<uint16_t>(Operation::SYNC);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(ptr, &req.note_id, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        std::memcpy(ptr, &req.user_id, sizeof(req.user_id));
        ptr += sizeof(req.user_id);

        std::memcpy(ptr, &base_text_size, sizeof(base_text_size));
        ptr += sizeof(base_text_size);

        std::memcpy(ptr, &text_size, sizeof(text_size));
        ptr += sizeof(text_size);

        if (base_text_size > 0) {
            std::memcpy(ptr, req.base_version.data(), base_text_size);
            ptr += base_text_size;
        }

        if (text_size > 0) {
            std::memcpy(ptr, req.local_version.data(), text_size);
        }

        return buffer;
    }

    std::vector<uint8_t> encodeSyncResponse(const SyncNoteResponse& resp) {
        const auto textSize = static_cast<uint32_t>(resp.text.size());
        std::vector<uint8_t> buffer(2 + 1 + 4 + 4 + 4 + textSize);
        uint8_t* ptr = buffer.data();

        const auto op = static_cast<uint16_t>(resp.op);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(ptr, &resp.status, sizeof(resp.status));
        ptr += sizeof(resp.status);

        std::memcpy(ptr, &resp.note_id, sizeof(resp.note_id));
        ptr += sizeof(resp.note_id);

        std::memcpy(ptr, &resp.version, sizeof(resp.version));
        ptr += sizeof(resp.version);

        std::memcpy(ptr, &textSize, sizeof(textSize));
        ptr += sizeof(textSize);

        if (textSize > 0) {
            std::memcpy(ptr, resp.text.data(), textSize);
        }

        return buffer;
    }

    std::optional<SyncNoteRequest> decodeSyncRequest(const std::vector<uint8_t>& buffer) {
        if (buffer.size() < (2 + 4 + 4 + 4 + 4))
            return std::nullopt;

        SyncNoteRequest request;
        const uint8_t* ptr = buffer.data();
        const uint8_t* end = buffer.data() + buffer.size();

        uint16_t op;
        std::memcpy(&op, ptr, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(&request.note_id, ptr, sizeof(request.note_id));
        ptr += sizeof(request.note_id);

        std::memcpy(&request.user_id, ptr, sizeof(request.user_id));
        ptr += sizeof(request.user_id);

        uint32_t base_text_size;
        std::memcpy(&base_text_size, ptr, sizeof(base_text_size));
        ptr += sizeof(base_text_size);

        uint32_t text_size;
        std::memcpy(&text_size, ptr, sizeof(text_size));
        ptr += sizeof(text_size);

        request.base_version.assign(reinterpret_cast<const char*>(ptr), base_text_size);
        ptr += base_text_size;

        request.local_version.assign(reinterpret_cast<const char*>(ptr), text_size);

        return request;
    }

    std::optional<SyncNoteResponse> decodeSyncResponse(const std::vector<uint8_t>& buffer) {

        SyncNoteResponse response;
        const uint8_t* ptr = buffer.data();

        uint16_t op_raw;
        std::memcpy(&op_raw, ptr, sizeof(op_raw));
        response.op = static_cast<Operation>(op_raw);
        ptr += sizeof(op_raw);

        std::memcpy(&response.status, ptr, sizeof(response.status));
        ptr += sizeof(response.status);

        std::memcpy(&response.note_id, ptr, sizeof(response.note_id));
        ptr += sizeof(response.note_id);

        std::memcpy(&response.version, ptr, sizeof(response.version));
        ptr += sizeof(response.version);

        uint32_t textLen;
        std::memcpy(&textLen, ptr, sizeof(textLen));
        ptr += sizeof(textLen);

        response.text.assign(reinterpret_cast<const char*>(ptr), textLen);

        return response;
    }

    std::vector<uint8_t> encodeGetNotesRequest(const GetNotesRequest& req) {
        std::vector<uint8_t> buffer(6);
        uint8_t* ptr = buffer.data();
        uint16_t opCode = static_cast<uint16_t>(Protocol::Operation::GET_NOTES);
        std::memcpy(ptr, &opCode, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        std::memcpy(ptr, &req.user_id, sizeof(uint32_t));

        return buffer;
    }

    std::vector<uint8_t> encodeGetNotesResponse(const GetNotesResponse& response) {
        std::vector<uint8_t> buffer;
        size_t total_size = sizeof(uint16_t) +
                            sizeof(uint32_t);

        for (const auto& [id, note_data] : response.notes) {
            const auto& [title, is_shared, version, owner_id] = note_data;
            total_size += sizeof(uint32_t) +
                          sizeof(uint32_t) +
                          sizeof(uint8_t) +
                          sizeof(uint32_t) +
                          sizeof(uint32_t) +
                          title.size();
        }

        buffer.reserve(total_size);

        uint16_t op_code = static_cast<uint16_t>(response.op);
        buffer.resize(sizeof(op_code));
        std::memcpy(buffer.data(), &op_code, sizeof(op_code));

        uint32_t notes_count = static_cast<uint32_t>(response.notes.size());
        size_t old_size = buffer.size();
        buffer.resize(old_size + sizeof(notes_count));
        std::memcpy(buffer.data() + old_size, &notes_count, sizeof(notes_count));

        for (const auto& [id, note_data] : response.notes) {
            const auto& [title, is_shared, version, owner_id] = note_data;

            old_size = buffer.size();
            buffer.resize(old_size + sizeof(uint32_t));
            std::memcpy(buffer.data() + old_size, &id, sizeof(uint32_t));

            old_size = buffer.size();
            buffer.resize(old_size + sizeof(uint32_t));
            std::memcpy(buffer.data() + old_size, &version, sizeof(uint32_t));

            old_size = buffer.size();
            buffer.resize(old_size + sizeof(uint8_t));
            uint8_t is_shared_byte = is_shared ? 1 : 0;
            std::memcpy(buffer.data() + old_size, &is_shared_byte, sizeof(uint8_t));

            old_size = buffer.size();
            buffer.resize(old_size + sizeof(uint32_t));
            std::memcpy(buffer.data() + old_size, &owner_id, sizeof(uint32_t));

            uint32_t title_len = static_cast<uint32_t>(title.size());
            old_size = buffer.size();
            buffer.resize(old_size + sizeof(uint32_t));
            std::memcpy(buffer.data() + old_size, &title_len, sizeof(uint32_t));

            buffer.insert(buffer.end(), title.begin(), title.end());
        }
        return buffer;
    }

    std::optional<GetNotesRequest> decodeGetNotesRequest(const std::vector<uint8_t>& buffer) {
        const uint8_t* ptr = buffer.data();
        uint16_t opCode;
        std::memcpy(&opCode, ptr, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        if (opCode != static_cast<uint16_t>(Protocol::Operation::GET_NOTES)) {
            return std::nullopt;
        }

        GetNotesRequest req;
        std::memcpy(&req.user_id, ptr, sizeof(uint32_t));

        return req;
    }

    std::optional<GetNotesResponse> decodeGetNotesResponse(const std::vector<uint8_t>& buffer) {
        if (buffer.size() < 6) {
            return std::nullopt;
        }

        GetNotesResponse response;
        size_t offset = 0;

        uint16_t op_code;
        std::memcpy(&op_code, buffer.data() + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        response.op = static_cast<Operation>(op_code);

        if (op_code != static_cast<uint16_t>(Operation::GET_NOTES)) {
            return std::nullopt;
        }

        uint32_t notes_count;
        if (offset + sizeof(uint32_t) > buffer.size()) {
            return std::nullopt;
        }
        std::memcpy(&notes_count, buffer.data() + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        for (uint32_t i = 0; i < notes_count; ++i) {
            if (offset + sizeof(uint32_t) > buffer.size()) {
                return std::nullopt;
            }
            uint32_t id;
            std::memcpy(&id, buffer.data() + offset, sizeof(uint32_t));
            offset += sizeof(uint32_t);

            if (offset + sizeof(uint32_t) > buffer.size()) {
                return std::nullopt;
            }
            uint32_t version;
            std::memcpy(&version, buffer.data() + offset, sizeof(uint32_t));
            offset += sizeof(uint32_t);

            if (offset + sizeof(uint8_t) > buffer.size()) {
                return std::nullopt;
            }
            uint8_t is_shared_byte;
            std::memcpy(&is_shared_byte, buffer.data() + offset, sizeof(uint8_t));
            offset += sizeof(uint8_t);
            bool is_shared = (is_shared_byte != 0);

            if (offset + sizeof(uint32_t) > buffer.size()) {
                return std::nullopt;
            }
            uint32_t owner_id;
            std::memcpy(&owner_id, buffer.data() + offset, sizeof(uint32_t));
            offset += sizeof(uint32_t);

            if (offset + sizeof(uint32_t) > buffer.size()) {
                return std::nullopt;
            }
            uint32_t title_len;
            std::memcpy(&title_len, buffer.data() + offset, sizeof(uint32_t));
            offset += sizeof(uint32_t);

            if (offset + title_len > buffer.size()) {
                return std::nullopt;
            }

            std::string title;
            if (title_len > 0) {
                title.assign(reinterpret_cast<const char*>(buffer.data() + offset), title_len);
            }
            offset += title_len;

            response.notes[id] = std::make_tuple(title, is_shared, version, owner_id);
        }

        return response;
    }

    std::vector<uint8_t> encodeCreateNoteRequest(const CreateNoteRequest& req) {
        const auto op = static_cast<uint16_t>(Operation::CREATE_NOTE);
        const std::string& title = req.note_title;
        const auto titleLen = static_cast<uint16_t>(title.size());

        const size_t totalSize =
                sizeof(uint16_t) +
                sizeof(uint32_t) +
                sizeof(uint16_t) +
                titleLen;

        std::vector<uint8_t> buffer(totalSize);
        uint8_t* ptr = buffer.data();

        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(ptr, &req.user_id, sizeof(req.user_id));
        ptr += sizeof(req.user_id);

        std::memcpy(ptr, &titleLen, sizeof(titleLen));
        ptr += sizeof(titleLen);

        std::memcpy(ptr, title.data(), titleLen);

        return buffer;
    }


    std::vector<uint8_t> encodeCreateNoteResponse(const CreateNoteResponse& resp) {
        const uint16_t titleLen = static_cast<uint16_t>(resp.note_title.size());
        std::vector<uint8_t> buffer(2 + 1 + 4 + 4 + 2 + titleLen);
        uint8_t* ptr = buffer.data();

        uint16_t op = static_cast<uint16_t>(resp.op);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(ptr, &resp.status, sizeof(resp.status));
        ptr += sizeof(resp.status);

        std::memcpy(ptr, &resp.note_id, sizeof(resp.note_id));
        ptr += sizeof(resp.note_id);

        std::memcpy(ptr, &resp.version, sizeof(resp.version));
        ptr += sizeof(resp.version);

        uint16_t titleLenNet = titleLen;
        std::memcpy(ptr, &titleLenNet, sizeof(titleLenNet));
        ptr += sizeof(titleLenNet);

        std::memcpy(ptr, resp.note_title.data(), titleLen);

        return buffer;
    }

    std::optional<CreateNoteRequest> decodeCreateNoteRequest(const std::vector<uint8_t>& buffer) {
        constexpr size_t MIN_SIZE =
                sizeof(uint16_t) +
                sizeof(uint32_t) +
                sizeof(uint16_t);

        if (buffer.size() < MIN_SIZE) {
            return std::nullopt;
        }

        const uint8_t* ptr = buffer.data();

        uint16_t op;
        std::memcpy(&op, ptr, sizeof(op));
        ptr += sizeof(op);

        if (op != static_cast<uint16_t>(Operation::CREATE_NOTE)) {
            return std::nullopt;
        }

        uint32_t userId;
        std::memcpy(&userId, ptr, sizeof(userId));
        ptr += sizeof(userId);

        uint16_t titleLen;
        std::memcpy(&titleLen, ptr, sizeof(titleLen));
        ptr += sizeof(titleLen);

        if (buffer.size() < MIN_SIZE + titleLen) {
            return std::nullopt;
        }

        std::string title(
                reinterpret_cast<const char*>(ptr),
                titleLen
        );

        return CreateNoteRequest{
                .user_id = userId,
                .note_title = std::move(title)
        };
    }

    std::optional<CreateNoteResponse> decodeCreateNoteResponse(const std::vector<uint8_t>& buffer) {
        if (buffer.size() < 2 + 1 + 4 + 4 + 2)
            return std::nullopt;

        CreateNoteResponse resp{};
        const uint8_t* ptr = buffer.data();
        const uint8_t* end = buffer.data() + buffer.size();

        uint16_t op;
        std::memcpy(&op, ptr, sizeof(op));
        resp.op = static_cast<Operation>(op);
        ptr += sizeof(op);

        std::memcpy(&resp.status, ptr, sizeof(resp.status));
        ptr += sizeof(resp.status);

        std::memcpy(&resp.note_id, ptr, sizeof(resp.note_id));
        ptr += sizeof(resp.note_id);

        std::memcpy(&resp.version, ptr, sizeof(resp.version));
        ptr += sizeof(resp.version);

        uint16_t titleLen;
        std::memcpy(&titleLen, ptr, sizeof(titleLen));
        titleLen = titleLen;
        ptr += sizeof(titleLen);

        if (ptr + titleLen > end)
            return std::nullopt;

        resp.note_title.assign(reinterpret_cast<const char*>(ptr), titleLen);

        return resp;
    }

    std::vector<uint8_t> encodeOpenNoteRequest(const OpenNoteRequest& req) {
        std::vector<uint8_t> buffer(2 + 4 + 4);
        uint8_t* ptr = buffer.data();

        uint16_t op = static_cast<uint16_t>(Operation::OPEN_NOTE);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(ptr, &req.user_id, sizeof(req.user_id));
        ptr += sizeof(req.user_id);

        std::memcpy(ptr, &req.note_id, sizeof(req.note_id));

        return buffer;
    }

    std::vector<uint8_t> encodeOpenNoteResponse(const OpenNoteResponse& resp) {
        uint32_t title_len = static_cast<uint32_t>(resp.title.size());
        uint32_t text_len = static_cast<uint32_t>(resp.text.size());
        std::vector<uint8_t> buffer(2 + 1 + 4 + 4 + 4 + 4 + title_len + text_len);
        uint8_t* ptr = buffer.data();

        uint16_t op = static_cast<uint16_t>(resp.op);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(ptr, &resp.status, sizeof(resp.status));
        ptr += sizeof(resp.status);

        std::memcpy(ptr, &resp.note_id, sizeof(resp.note_id));
        ptr += sizeof(resp.note_id);

        std::memcpy(ptr, &resp.version, sizeof(resp.version));
        ptr += sizeof(resp.version);

        std::memcpy(ptr, &title_len, sizeof(title_len));
        ptr += sizeof(title_len);

        std::memcpy(ptr, &text_len, sizeof(text_len));
        ptr += sizeof(text_len);

        if (title_len > 0) {
            std::memcpy(ptr, resp.title.data(), title_len);
            ptr += title_len;
        }

        if (text_len > 0) {
            std::memcpy(ptr, resp.text.data(), text_len);
        }

        return buffer;
    }

    std::optional<OpenNoteRequest> decodeOpenNoteRequest(const std::vector<uint8_t>& buffer) {
        constexpr size_t REQUIRED_SIZE = sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t);
        if (buffer.size() < REQUIRED_SIZE) {
            return std::nullopt;
        }

        const uint8_t* ptr = buffer.data();
        uint16_t op;
        std::memcpy(&op, ptr, sizeof(op));
        ptr += sizeof(op);

        if (op != static_cast<uint16_t>(Operation::OPEN_NOTE)) {
            return std::nullopt;
        }

        OpenNoteRequest req;
        std::memcpy(&req.user_id, ptr, sizeof(req.user_id));
        ptr += sizeof(req.user_id);

        std::memcpy(&req.note_id, ptr, sizeof(req.note_id));

        return req;
    }

    std::optional<OpenNoteResponse> decodeOpenNoteResponse(const std::vector<uint8_t>& buffer) {
        constexpr size_t MIN_SIZE = 2 + 1 + 4 + 4 + 4 + 4;
        if (buffer.size() < MIN_SIZE) {
            return std::nullopt;
        }

        const uint8_t* ptr = buffer.data();
        uint16_t op;
        std::memcpy(&op, ptr, sizeof(op));
        ptr += sizeof(op);

        if (op != static_cast<uint16_t>(Operation::OPEN_NOTE)) {
            return std::nullopt;
        }

        OpenNoteResponse resp;
        resp.op = Operation::OPEN_NOTE;

        std::memcpy(&resp.status, ptr, sizeof(resp.status));
        ptr += sizeof(resp.status);

        std::memcpy(&resp.note_id, ptr, sizeof(resp.note_id));
        ptr += sizeof(resp.note_id);

        std::memcpy(&resp.version, ptr, sizeof(resp.version));
        ptr += sizeof(resp.version);

        uint32_t title_len;
        std::memcpy(&title_len, ptr, sizeof(title_len));
        ptr += sizeof(title_len);

        uint32_t text_len;
        std::memcpy(&text_len, ptr, sizeof(text_len));
        ptr += sizeof(text_len);

        size_t expected_size = MIN_SIZE + title_len + text_len;
        if (buffer.size() < expected_size) {
            return std::nullopt;
        }

        if (title_len > 0) {
            resp.title.assign(reinterpret_cast<const char*>(ptr), title_len);
            ptr += title_len;
        }

        if (text_len > 0) {
            resp.text.assign(reinterpret_cast<const char*>(ptr), text_len);
        }

        return resp;
    }


    std::vector<uint8_t> encodeUpdateTextRequest(const UpdateTextRequest& req) {
        std::string text = req.text;
        uint32_t text_len = static_cast<uint32_t>(text.size());
        std::vector<uint8_t> buffer(2 + 4 + 4 + 4 + 4 + text_len);
        uint8_t* ptr = buffer.data();

        uint16_t op = static_cast<uint16_t>(Operation::UPDATE_TEXT);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(ptr, &req.user_id, sizeof(req.user_id));
        ptr += sizeof(req.user_id);

        std::memcpy(ptr, &req.note_id, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        std::memcpy(ptr, &req.version, sizeof(req.version));
        ptr += sizeof(req.version);

        std::memcpy(ptr, &text_len, sizeof(text_len));
        ptr += sizeof(text_len);

        std::memcpy(ptr, text.data(), text_len);

        return buffer;
    }

    std::vector<uint8_t> encodeUpdateTextResponse(const UpdateTextResponse& resp) {
        std::vector<uint8_t> buffer(2 + 1 + 4 + 4);
        uint8_t* ptr = buffer.data();

        uint16_t op = static_cast<uint16_t>(resp.op);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(ptr, &resp.status, sizeof(resp.status));
        ptr += sizeof(resp.status);

        std::memcpy(ptr, &resp.note_id, sizeof(resp.note_id));
        ptr += sizeof(resp.note_id);

        std::memcpy(ptr, &resp.version, sizeof(resp.version));

        return buffer;
    }

    std::optional<UpdateTextRequest> decodeUpdateTextRequest(const std::vector<uint8_t>& buffer) {
        const uint8_t* ptr = buffer.data();
        uint16_t op;
        std::memcpy(&op, ptr, sizeof(op));
        ptr += sizeof(op);

        UpdateTextRequest req;

        std::memcpy(&req.user_id, ptr, sizeof(req.user_id));
        ptr += sizeof(req.user_id);

        std::memcpy(&req.note_id, ptr, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        std::memcpy(&req.version, ptr, sizeof(req.version));
        ptr += sizeof(req.version);

        uint32_t text_len;
        std::memcpy(&text_len, ptr, sizeof(text_len));
        ptr += sizeof(text_len);

        std::string text(reinterpret_cast<const char*>(ptr), text_len);
        req.text = std::move(text);
        return req;
    }

    std::optional<UpdateTextResponse> decodeUpdateTextResponse(const std::vector<uint8_t>& buffer) {
        const uint8_t* ptr = buffer.data();

        uint16_t op;
        std::memcpy(&op, ptr, sizeof(op));
        ptr += sizeof(op);

        UpdateTextResponse resp;
        resp.op = Operation::UPDATE_TEXT;

        std::memcpy(&resp.status, ptr, sizeof(resp.status));
        ptr += sizeof(resp.status);

        std::memcpy(&resp.note_id, ptr, sizeof(resp.note_id));
        ptr += sizeof(resp.note_id);

        std::memcpy(&resp.version, ptr, sizeof(resp.version));

        return resp;
    }

    std::vector<uint8_t> encodeShareNoteRequest(const ShareNoteRequest& req) {
        std::vector<uint8_t> buffer(2 + 4 + 4);
        uint8_t* ptr = buffer.data();
        uint16_t op = static_cast<uint16_t>(Operation::SHARE_NOTE);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(ptr, &req.user_id, sizeof(req.user_id));
        ptr += sizeof(req.user_id);

        std::memcpy(ptr, &req.note_id, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        std::memcpy(ptr, &req.version, sizeof(req.version));

        return buffer;
    }

    std::optional<ShareNoteRequest> decodeShareNoteRequest(const std::vector<uint8_t>& buffer) {
        ShareNoteRequest req;
        const uint8_t* ptr = buffer.data();

        Protocol::Operation op;
        uint16_t op_raw;
        std::memcpy(&op_raw, ptr, sizeof(op_raw));
        ptr += sizeof(op_raw);

        std::memcpy(&req.user_id, ptr, sizeof(req.user_id));
        ptr += sizeof(req.user_id);

        std::memcpy(&req.note_id, ptr, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        std::memcpy(&req.version, ptr, sizeof(req.version));
        return req;
    }

    std::vector<uint8_t> encodeShareNoteNotifyRequest(const ShareNoteNotifyRequest& req) {
        uint32_t title_len = static_cast<uint32_t>(req.note_title.size());

        std::vector<uint8_t> buffer(2 + 4 + 4 + 4 + 4 + title_len);
        uint8_t* ptr = buffer.data();

        std::memcpy(ptr, &req.op, sizeof(req.op));
        ptr += sizeof(req.op);

        std::memcpy(ptr, &req.note_id, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        std::memcpy(ptr, &req.owner_id, sizeof(req.owner_id));
        ptr += sizeof(req.owner_id);

        std::memcpy(ptr, &req.version, sizeof(req.version));
        ptr += sizeof(req.version);

        std::memcpy(ptr, &title_len, sizeof(title_len));
        ptr += sizeof(title_len);

        if (title_len > 0) {
            std::memcpy(ptr, req.note_title.data(), title_len);
        }

        return buffer;
    }

    std::optional<ShareNoteNotifyRequest> decodeShareNoteNotifyRequest(const std::vector<uint8_t>& buffer) {
        constexpr size_t MIN_SIZE = 2 + 4 + 4 + 4;
        if (buffer.size() < MIN_SIZE) {
            return std::nullopt;
        }

        const uint8_t* ptr = buffer.data();
        ShareNoteNotifyRequest req;

        uint16_t op;
        std::memcpy(&op, ptr, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(&req.note_id, ptr, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        std::memcpy(&req.owner_id, ptr, sizeof(req.owner_id));
        ptr += sizeof(req.owner_id);

        std::memcpy(&req.version, ptr, sizeof(req.version));
        ptr += sizeof(req.version);

        uint32_t title_len;
        std::memcpy(&title_len, ptr, sizeof(title_len));
        ptr += sizeof(title_len);

        size_t expected_size = MIN_SIZE + title_len;
        if (buffer.size() < expected_size) {
            return std::nullopt;
        }

        if (title_len > 0) {
            req.note_title.assign(reinterpret_cast<const char*>(ptr), title_len);
        }

        return req;
    }

    std::vector<uint8_t> encodeApproveMergeRequest(const ApproveMergeRequest& req) {
        const uint32_t text_size = static_cast<uint32_t>(req.merged_text.size());
        std::vector<uint8_t> buffer(2 + 4 + 4 + 4 + 1 + text_size);
        uint8_t* ptr = buffer.data();
        uint16_t op = static_cast<uint16_t>(Protocol::Operation::APPROVE_MERGE);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(ptr, &req.note_id, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        std::memcpy(ptr, &req.user_id, sizeof(req.user_id));
        ptr += sizeof(req.user_id);

        std::memcpy(ptr, &req.status, sizeof(req.status));
        ptr += sizeof(req.status);

        std::memcpy(ptr, &text_size, sizeof(text_size));
        ptr += sizeof(text_size);

        std::memcpy(ptr, req.merged_text.data(), text_size);

        return buffer;
    }

    std::optional<ApproveMergeRequest> decodeApproveMergeRequest(const std::vector<uint8_t>& buffer) {
        ApproveMergeRequest req;
        const uint8_t* ptr = buffer.data();
        uint16_t op;
        std::memcpy(&op, ptr, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(&req.note_id, ptr, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        std::memcpy(&req.user_id, ptr, sizeof(req.user_id));
        ptr += sizeof(req.user_id);

        std::memcpy(&req.status, ptr, sizeof(req.status));
        ptr += sizeof(req.status);

        uint32_t text_size;
        std::memcpy(&text_size, ptr, sizeof(text_size));
        ptr += sizeof(text_size);

        req.merged_text.assign(reinterpret_cast<const char*>(ptr), text_size);

        return req;
    }

    std::vector<uint8_t> encodeOwnerApproveMergeRequest(const OwnerApproveMergeRequest& req) {
        const uint32_t text_size = static_cast<uint32_t>(req.approve_text.size());
        std::vector<uint8_t> buffer(2 + 4 + 4 + 4 + text_size);
        uint8_t* ptr = buffer.data();

        uint16_t op = static_cast<uint16_t>(Protocol::Operation::OWNER_APPROVE_MERGE);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(ptr, &req.note_id, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        std::memcpy(ptr, &req.merge_sender_id, sizeof(req.merge_sender_id));
        ptr += sizeof(req.merge_sender_id);

        std::memcpy(ptr, &text_size, sizeof(text_size));
        ptr += sizeof(text_size);

        if (text_size > 0) {
            std::memcpy(ptr, req.approve_text.data(), text_size);
        }

        return buffer;
    }

    std::vector<uint8_t> encodeOwnerApproveMergeResponse(const OwnerApproveMergeResponse& resp) {
        const uint32_t text_size = static_cast<uint32_t>(resp.approved_text.size());
        std::vector<uint8_t> buffer(2 + 4 + 4 + 4 + 1 + 4 + text_size);
        uint8_t* ptr = buffer.data();

        uint16_t op = static_cast<uint16_t>(Operation::OWNER_APPROVE_MERGE);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(ptr, &resp.note_id, sizeof(resp.note_id));
        ptr += sizeof(resp.note_id);

        std::memcpy(ptr, &resp.sender_id, sizeof(resp.sender_id));
        ptr += sizeof(resp.sender_id);

        std::memcpy(ptr, &resp.new_version, sizeof(resp.new_version));
        ptr += sizeof(resp.new_version);

        std::memcpy(ptr, &resp.status, sizeof(resp.status));
        ptr += sizeof(resp.status);

        std::memcpy(ptr, &text_size, sizeof(text_size));
        ptr += sizeof(text_size);

        if (text_size > 0) {
            std::memcpy(ptr, resp.approved_text.data(), text_size);
        }

        return buffer;
    }

    std::optional<OwnerApproveMergeRequest> decodeOwnerApproveMergeRequest(const std::vector<uint8_t>& buffer) {
        OwnerApproveMergeRequest req{};
        if (buffer.size() < 2 + 4 + 4 + 4)
            return std::nullopt;

        const uint8_t* ptr = buffer.data();
        const uint8_t* end = ptr + buffer.size();

        uint16_t op = 0;
        std::memcpy(&op, ptr, sizeof(op));
        ptr += sizeof(op);

        if (op != static_cast<uint16_t>(Protocol::Operation::OWNER_APPROVE_MERGE))
            return std::nullopt;

        if (ptr + sizeof(req.note_id) > end) return std::nullopt;
        std::memcpy(&req.note_id, ptr, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        if (ptr + sizeof(req.merge_sender_id) > end) return std::nullopt;
        std::memcpy(&req.merge_sender_id, ptr, sizeof(req.merge_sender_id));
        ptr += sizeof(req.merge_sender_id);

        uint32_t text_size = 0;
        if (ptr + sizeof(text_size) > end) return std::nullopt;
        std::memcpy(&text_size, ptr, sizeof(text_size));
        ptr += sizeof(text_size);

        if (ptr + text_size > end) return std::nullopt;
        req.approve_text.assign(reinterpret_cast<const char*>(ptr), text_size);

        return req;
    }

    std::optional<OwnerApproveMergeResponse> decodeOwnerApproveMergeResponse(const std::vector<uint8_t>& buffer) {
        constexpr size_t MIN_SIZE = 2 + 4 + 4 + 4 + 1 + 4;
        if (buffer.size() < MIN_SIZE) {
            return std::nullopt;
        }

        OwnerApproveMergeResponse resp{};
        const uint8_t* ptr = buffer.data();

        uint16_t op_raw = 0;
        std::memcpy(&op_raw, ptr, sizeof(op_raw));
        ptr += sizeof(op_raw);

        if (static_cast<Operation>(op_raw) != Operation::OWNER_APPROVE_MERGE) {
            return std::nullopt;
        }

        std::memcpy(&resp.note_id, ptr, sizeof(resp.note_id));
        ptr += sizeof(resp.note_id);

        std::memcpy(&resp.sender_id, ptr, sizeof(resp.sender_id));
        ptr += sizeof(resp.sender_id);

        std::memcpy(&resp.new_version, ptr, sizeof(resp.new_version));
        ptr += sizeof(resp.new_version);

        std::memcpy(&resp.status, ptr, sizeof(resp.status));
        ptr += sizeof(resp.status);

        uint32_t text_len = 0;
        std::memcpy(&text_len, ptr, sizeof(text_len));
        ptr += sizeof(text_len);

        const size_t used = static_cast<size_t>(ptr - buffer.data());
        if (buffer.size() < used + static_cast<size_t>(text_len)) {
            return std::nullopt;
        }

        if (text_len > 0) {
            resp.approved_text.assign(reinterpret_cast<const char*>(ptr), text_len);
        }

        return resp;
    }

    std::vector<uint8_t> encodeServerApproveMergeRequest(const ServerApproveMergeRequest& req) {
        const uint32_t str_size = static_cast<uint32_t>(req.server_version.size());
        std::vector<uint8_t> buffer(2 + 4 + 4 + 4 + str_size);
        uint8_t* ptr = buffer.data();

        uint16_t op = static_cast<uint16_t>(Protocol::Operation::SERVER_APPROVE_MERGE);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(ptr, &req.note_id, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        std::memcpy(ptr, &req.version, sizeof(req.version));
        ptr += sizeof(req.version);

        std::memcpy(ptr, &str_size, sizeof(str_size));
        ptr += sizeof(str_size);

        if (str_size > 0) {
            std::memcpy(ptr, req.server_version.data(), str_size);
        }

        return buffer;
    }

    std::optional<ServerApproveMergeRequest> decodeServerApproveMergeRequest(const std::vector<uint8_t>& buffer) {
        constexpr size_t MIN_SIZE = 2 + 4 + 4 + 4;
        if (buffer.size() < MIN_SIZE)
            return std::nullopt;

        ServerApproveMergeRequest req{};
        const uint8_t* ptr = buffer.data();

        uint16_t op_raw = 0;
        std::memcpy(&op_raw, ptr, sizeof(op_raw));
        ptr += sizeof(op_raw);

        if (static_cast<Protocol::Operation>(op_raw) != Protocol::Operation::SERVER_APPROVE_MERGE) {
            return std::nullopt;
        }

        std::memcpy(&req.note_id, ptr, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        std::memcpy(&req.version, ptr, sizeof(req.version));
        ptr += sizeof(req.version);

        uint32_t str_len = 0;
        std::memcpy(&str_len, ptr, sizeof(str_len));
        ptr += sizeof(str_len);

        const size_t used = static_cast<size_t>(ptr - buffer.data());
        if (buffer.size() < used + static_cast<size_t>(str_len))
            return std::nullopt;

        req.server_version.assign(reinterpret_cast<const char*>(ptr), str_len);

        return req;
    }

    std::vector<uint8_t> encodeUpdateTextMergedRequest(const UpdateTextMergedRequest& req) {
        const uint32_t text_size = static_cast<uint32_t>(req.merged_version.size());
        std::vector<uint8_t> buffer(2 + 4 + 4 + 4 + text_size);
        uint8_t* ptr = buffer.data();

        uint16_t op = static_cast<uint16_t>(Protocol::Operation::UPDATE_TEXT_MERGED);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(ptr, &req.note_id, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        std::memcpy(ptr, &req.version, sizeof(req.version));
        ptr += sizeof(req.version);

        std::memcpy(ptr, &text_size, sizeof(text_size));
        ptr += sizeof(text_size);

        if (text_size > 0) {
            std::memcpy(ptr, req.merged_version.data(), text_size);
        }

        return buffer;
    }

    std::optional<UpdateTextMergedRequest> decodeUpdateTextMergedRequest(const std::vector<uint8_t>& buffer) {
        constexpr size_t MIN_SIZE = 2 + 4 + 4 + 4;

        if (buffer.size() < MIN_SIZE) {
            return std::nullopt;
        }

        UpdateTextMergedRequest req{};
        const uint8_t* ptr = buffer.data();

        uint16_t op_raw = 0;
        std::memcpy(&op_raw, ptr, sizeof(op_raw));
        ptr += sizeof(op_raw);

        if (static_cast<Protocol::Operation>(op_raw) != Protocol::Operation::UPDATE_TEXT_MERGED) {
            return std::nullopt;
        }

        std::memcpy(&req.note_id, ptr, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        std::memcpy(&req.version, ptr, sizeof(req.version));
        ptr += sizeof(req.version);

        uint32_t text_len = 0;
        std::memcpy(&text_len, ptr, sizeof(text_len));
        ptr += sizeof(text_len);

        const size_t used = static_cast<size_t>(ptr - buffer.data());
        if (buffer.size() < used + static_cast<size_t>(text_len)) {
            return std::nullopt;
        }

        if (text_len > 0) {
            req.merged_version.assign(reinterpret_cast<const char*>(ptr), text_len);
        }

        return req;
    }
}
