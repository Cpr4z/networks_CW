#include "Protocol.hpp"

#include <iostream>

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

        // Рассчитываем общий размер сообщения
        size_t totalSize = 2; // Код операции
        totalSize += 2; // Длина логина
        totalSize += 2; // Длина пароля
        totalSize += loginLen; // Логин
        totalSize += passwordLen; // Пароль

        std::vector<uint8_t> buffer(totalSize);
        uint8_t* ptr = buffer.data();

        auto opCode = static_cast<uint16_t>(Protocol::Operation::AUTH);
        std::memcpy(ptr, &opCode, 2);
        ptr += 2;

        // Кодируем длину логина
        std::memcpy(ptr, &loginLen, 2);
        ptr += 2;

        // Кодируем длину пароля
        std::memcpy(ptr, &passwordLen, 2);
        ptr += 2;

        // Кодируем логин (без завершающего нуля)
        std::memcpy(ptr, login.data(), loginLen);
        ptr += loginLen;

        // Кодируем пароль (без завершающего нуля)
        std::memcpy(ptr, password.data(), passwordLen);

        return buffer;
    }

    std::vector<uint8_t> encodeAuthResponse(const AuthResponse& resp) {
        std::vector<uint8_t> buffer(7);
        uint8_t* ptr = buffer.data();

        // Код операции
        uint16_t op = static_cast<uint16_t>(resp.op);
//        uint16_t op = htons(static_cast<uint16_t>(resp.op));
        std::memcpy(ptr, &op, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        // Статус
        std::memcpy(ptr, &resp.status, sizeof(uint8_t));
        ptr += sizeof(uint8_t);

        // ID пользователя
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

        // 1. opcode
        std::memcpy(&response.op, ptr, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        // 2. status
        std::memcpy(&response.status, ptr, sizeof(uint8_t));
        ptr += sizeof(uint8_t);

        // 3. user_id
        std::memcpy(&response.user_id, ptr, sizeof(uint32_t));

        return response;
    }

    std::optional<AuthRequest> decodeAuthRequest(const std::vector<uint8_t>& buffer) {
        if (buffer.size() < 6) {
            return std::nullopt;
        }

        AuthRequest request;

        const uint8_t* ptr = buffer.data();

        // По идее код не нужно читать
        uint16_t opCode;
        std::memcpy(&opCode, ptr, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        // 2. Читаем длину логина (следующие 2 байта)
        uint16_t loginLen;
        std::memcpy(&loginLen, ptr, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        // 3. Читаем длину пароля (следующие 2 байта)
        uint16_t passwordLen;
        std::memcpy(&passwordLen, ptr, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        // Проверяем, что в буфере достаточно данных для логина и пароля
        size_t requiredSize = 6 + loginLen + passwordLen;  // 2+2+2 + login + password
        if (buffer.size() < requiredSize) {
            return std::nullopt;  // Не хватает данных
        }

        // 4. Читаем сам логин
        if (loginLen > 0) {
            request.login.assign(reinterpret_cast<const char*>(ptr), loginLen);
            ptr += loginLen;
        } else {
            request.login = "";  // Пустой логин
        }

        // 5. Читаем пароль
        if (passwordLen > 0) {
            request.password.assign(reinterpret_cast<const char*>(ptr), passwordLen);
            // ptr += passwordLen;  // Не нужно дальше двигать
        } else {
            request.password = "";  // Пустой пароль
        }

        return request;  // Возвращаем декодированную структуру
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

        // Рассчитываем общий размер сообщения
        size_t totalSize = 2; // Код операции
        totalSize += 2; // Длина логина
        totalSize += 2; // Длина пароля
        totalSize += loginLen; // Логин
        totalSize += passwordLen; // Пароль

        std::vector<uint8_t> buffer(totalSize);
        uint8_t* ptr = buffer.data();

        auto opCode = static_cast<uint16_t>(Protocol::Operation::REGISTRATION); // REGISTRATION
        std::cout << opCode << std::endl;
        std::memcpy(ptr, &opCode, 2);
        ptr += 2;

        // Кодируем длину логина
        std::memcpy(ptr, &loginLen, 2);
        ptr += 2;

        // Кодируем длину пароля
        std::memcpy(ptr, &passwordLen, 2);
        ptr += 2;

        // Кодируем логин (без завершающего нуля)
        std::memcpy(ptr, login.data(), loginLen);
        ptr += loginLen;

        // Кодируем пароль (без завершающего нуля)
        std::memcpy(ptr, password.data(), passwordLen);

        return buffer;
    }

    std::vector<uint8_t> encodeRegistrationResponse(const RegistrationResponse& resp) {
//        return {};
        std::vector<uint8_t> buffer(7);
        uint8_t* ptr = buffer.data();

        // Код операции
        uint16_t op = static_cast<uint16_t>(resp.op);
        std::memcpy(ptr, &op, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        // Статус
        std::memcpy(ptr, &resp.status, sizeof(uint8_t));
        ptr += sizeof(uint8_t);

        // ID пользователя
        std::memcpy(ptr, &resp.user_id, sizeof(uint32_t));

        return buffer;
    }


    std::optional<RegistrationRequest> decodeRegistrationRequest(const std::vector<uint8_t>& buffer) {
//        if (buffer.size() < 6) {
//            return std::nullopt;
//        }

        const uint8_t* ptr = buffer.data();

        uint16_t opCode = 0;
        std::memcpy(&opCode, ptr, 2);
        ptr += 2;

        if (static_cast<Protocol::Operation>(opCode) != Protocol::Operation::REGISTRATION) {
            return std::nullopt;
        }

        uint16_t loginLen = 0;
        std::memcpy(&loginLen, ptr, 2);
        ptr += 2;

        // Длина пароля
        uint16_t passwordLen = 0;
        std::memcpy(&passwordLen, ptr, 2);
        ptr += 2;

        // Проверка размеров
        if (buffer.size() < 6 + loginLen + passwordLen) {
            return std::nullopt;
        }

        // Логин
        std::string login(reinterpret_cast<const char*>(ptr), loginLen);
        ptr += loginLen;

        // Пароль
        std::string password(reinterpret_cast<const char*>(ptr), passwordLen);

        return RegistrationRequest{
                .login = std::move(login),
                .password = std::move(password)
        };
    }

    std::optional<RegistrationResponse> decodeRegistrationResponse(const std::vector<uint8_t>& buffer) {
        constexpr size_t MIN_SIZE = 7;
//        if (buffer.size() < MIN_SIZE) {
//            return std::nullopt;
//        }
        RegistrationResponse response;
        const uint8_t* ptr = buffer.data();

        std::memcpy(&response.op, ptr, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        // 2. Декодируем статус (1 байт)
        std::memcpy(&response.status, ptr, sizeof(uint8_t));
//        response.status = *ptr;
        ptr += sizeof(uint8_t);

        // 3. Декодируем идентификатор пользователя (только при успехе)
        response.user_id = 0; // По умолчанию 0
        if (response.status == 0) {
            // Проверяем, что буфер содержит достаточно данных для userId
//            if (buffer.size() < MIN_SIZE + sizeof(uint32_t)) {
//                return std::nullopt;
//            }
            std::memcpy(&response.user_id, ptr, sizeof(uint32_t));
//            ptr += sizeof(uint32_t);
        }
        return response;
    }


    std::vector<uint8_t> encodeSyncRequest(const SyncNoteRequest& req) {
        return {};
    }

    std::vector<uint8_t> encodeSyncResponse(const SyncNoteResponse& resp) {
        return {};
    }

    std::optional<SyncNoteRequest> decodeSyncRequest(const std::vector<uint8_t>& buffer) {
        return {};
    }

    std::optional<SyncNoteResponse> decodeSyncResponse(const std::vector<uint8_t>& buffer) {
        return {};
    }




    std::vector<uint8_t> encodeGetNotesRequest(const GetNotesRequest& req) {
        return {};
    }

    std::vector<uint8_t> encodeGetNotesResponse(const GetNotesResponse& response) {
        return {};
    }

    std::optional<GetNotesRequest> decodeGetNotesRequest(const std::vector<uint8_t>& buffer) {
        return {};
    }

    std::optional<GetNotesResponse> decodeGetNotesResponse(const std::vector<uint8_t>& buffer) {
        return {};
    }



    std::vector<uint8_t> encodeCreateNoteRequest(const CreateNoteRequest& req) {
        const auto op = static_cast<uint16_t>(Operation::CREATE_NOTE);
        const std::string& title = req.note_title;
        const auto titleLen = static_cast<uint16_t>(title.size());

        // Общий размер:
        // 1  — op
        // 4  — user_id
        // 2  — длина заголовка
        // N  — заголовок
        const size_t totalSize =
                sizeof(uint16_t) +
                sizeof(uint32_t) +
                sizeof(uint16_t) +
                titleLen;


        std::vector<uint8_t> buffer(totalSize);
        uint8_t* ptr = buffer.data();

        //  Код операции
        std::memcpy(ptr, &op, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        // user_id
        std::memcpy(ptr, &req.user_id, sizeof(uint32_t));
        ptr += sizeof(uint32_t);

        // длина заголовка
        std::memcpy(ptr, &titleLen, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        // сам заголовок
        std::memcpy(ptr, title.data(), titleLen);

        return buffer;
    }


    std::vector<uint8_t> encodeCreateNoteResponse(const CreateNoteResponse& resp) {
        const uint16_t titleLen = static_cast<uint16_t>(resp.note_title.size());

        // 2 (op) + 1 (status) + 4 (note_id) + 2 (titleLen) + title
        std::vector<uint8_t> buffer(2 + 1 + 4 + 2 + titleLen);
        uint8_t* ptr = buffer.data();

        uint16_t op = static_cast<uint16_t>(resp.op);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        std::memcpy(ptr, &resp.status, sizeof(resp.status));
        ptr += sizeof(resp.status);

//        uint32_t noteId = resp.note_id;
        std::memcpy(ptr, &resp.note_id, sizeof(resp.note_id));
        ptr += sizeof(resp.note_id);

        uint16_t titleLenNet = titleLen;
        std::memcpy(ptr, &titleLenNet, sizeof(titleLenNet));
        ptr += sizeof(titleLenNet);

        std::memcpy(ptr, resp.note_title.data(), titleLen);

        return buffer;
    }

//    std::vector<uint8_t> encodeCreateNoteResponse(const CreateNoteResponse& resp) {
//        const uint16_t titleLen = static_cast<uint16_t>(resp.note_title.size());
//
//        // 2 (op) + 1 (status) + 4 (note_id) + 2 (titleLen) + title
//        std::vector<uint8_t> buffer(2 + 1 + 4 + 2 + titleLen);
//        uint8_t* ptr = buffer.data();
//
//        // Преобразуем в сетевой порядок байт
//        uint16_t op = htons(static_cast<uint16_t>(resp.op));
//        std::memcpy(ptr, &op, sizeof(op));
//        ptr += sizeof(op);
//
//        std::memcpy(ptr, &resp.status, sizeof(resp.status));
//        ptr += sizeof(resp.status);
//
//        uint32_t noteId = htonl(resp.note_id);
//        std::memcpy(ptr, &noteId, sizeof(noteId));
//        ptr += sizeof(noteId);
//
//        uint16_t titleLenNet = htons(titleLen);
//        std::memcpy(ptr, &titleLenNet, sizeof(titleLenNet));
//        ptr += sizeof(titleLenNet);
//
//        std::memcpy(ptr, resp.note_title.data(), titleLen);
//
//        return buffer;
//    }

    std::optional<CreateNoteRequest> decodeCreateNoteRequest(const std::vector<uint8_t>& buffer) {
        constexpr size_t MIN_SIZE =
                sizeof(uint16_t) +   // op
                sizeof(uint32_t) +  // user_id
                sizeof(uint16_t);   // title_len

        if (buffer.size() < MIN_SIZE) {
            return std::nullopt;
        }

        const uint8_t* ptr = buffer.data();

        // 1️⃣ Код операции
        uint16_t op;
        std::memcpy(&op, ptr, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        if (op != static_cast<uint16_t>(Operation::CREATE_NOTE)) {
            return std::nullopt;
        }

        // 2️⃣ user_id
        uint32_t userId;
        std::memcpy(&userId, ptr, sizeof(uint32_t));
        ptr += sizeof(uint32_t);

        // 3️⃣ длина заголовка
        uint16_t titleLen;
        std::memcpy(&titleLen, ptr, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        // Проверка размера
        if (buffer.size() < MIN_SIZE + titleLen) {
            return std::nullopt;
        }

        // 4️⃣ заголовок заметки
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
        if (buffer.size() < 2 + 1 + 4 + 2)
            return std::nullopt;

        CreateNoteResponse resp{};
        const uint8_t* ptr = buffer.data();
        const uint8_t* end = buffer.data() + buffer.size();

        // 1. Operation
        uint16_t op;
        std::memcpy(&op, ptr, sizeof(op));
        resp.op = static_cast<Operation>(ntohs(op));
        ptr += sizeof(op);

        // 2. Status
        std::memcpy(&resp.status, ptr, sizeof(resp.status));
        ptr += sizeof(resp.status);

        // 3. Note ID
//        uint32_t noteId = resp.note_id;
        std::memcpy(&resp.note_id, ptr, sizeof(resp.note_id));
//        resp.note_id = ntohl(noteId);
        ptr += sizeof(resp.note_id);

        // 4. Title length
        uint16_t titleLen;
        std::memcpy(&titleLen, ptr, sizeof(titleLen));
        titleLen = titleLen;
        ptr += sizeof(titleLen);

        // 🔐 Проверка границ
        if (ptr + titleLen > end)
            return std::nullopt;

        // 5. Title
        resp.note_title.assign(reinterpret_cast<const char*>(ptr), titleLen);
        ptr += titleLen;

        return resp;
    }


    std::vector<uint8_t> encodeOpenNoteRequest(const OpenNoteRequest& req) {
        std::vector<uint8_t> buffer(2 + 4 + 4);
        uint8_t* ptr = buffer.data();

        // Код операции
        uint16_t op = static_cast<uint16_t>(Operation::OPEN_NOTE);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        // user_id
        std::memcpy(ptr, &req.user_id, sizeof(req.user_id));
        ptr += sizeof(req.user_id);

        // note_id
        std::memcpy(ptr, &req.note_id, sizeof(req.note_id));

        return buffer;
//        return {};
    }

    std::vector<uint8_t> encodeOpenNoteResponse(const OpenNoteResponse& resp) {
        uint32_t title_len = static_cast<uint32_t>(resp.title.size());
        uint32_t text_len = static_cast<uint32_t>(resp.text.size());

        // 2(op) + 1(status) + 4(note_id) + 4(title_len) + 4(text_len) + title + text
        std::vector<uint8_t> buffer(2 + 1 + 4 + 4 + 4 + title_len + text_len);
        uint8_t* ptr = buffer.data();

        // Код операции
        uint16_t op = static_cast<uint16_t>(resp.op);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        // Статус
        std::memcpy(ptr, &resp.status, sizeof(resp.status));
        ptr += sizeof(resp.status);

        // ID заметки
        std::memcpy(ptr, &resp.note_id, sizeof(resp.note_id));
        ptr += sizeof(resp.note_id);

        // Длина заголовка
        std::memcpy(ptr, &title_len, sizeof(title_len));
        ptr += sizeof(title_len);

        // Длина текста
        std::memcpy(ptr, &text_len, sizeof(text_len));
        ptr += sizeof(text_len);

        // Заголовок (если есть)
        if (title_len > 0) {
            std::memcpy(ptr, resp.title.data(), title_len);
            ptr += title_len;
        }

        // Текст (если есть)
        if (text_len > 0) {
            std::memcpy(ptr, resp.text.data(), text_len);
        }

        return buffer;
//        return {};
    }

    std::optional<OpenNoteRequest> decodeOpenNoteRequest(const std::vector<uint8_t>& buffer) {
        constexpr size_t REQUIRED_SIZE = sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t);

        if (buffer.size() < REQUIRED_SIZE) {
            return std::nullopt;
        }

        const uint8_t* ptr = buffer.data();

        // Проверяем код операции
        uint16_t op;
        std::memcpy(&op, ptr, sizeof(op));
        ptr += sizeof(op);

        if (op != static_cast<uint16_t>(Operation::OPEN_NOTE)) {
            return std::nullopt;
        }

        OpenNoteRequest req;

        // user_id
        std::memcpy(&req.user_id, ptr, sizeof(req.user_id));
        ptr += sizeof(req.user_id);

        // note_id
        std::memcpy(&req.note_id, ptr, sizeof(req.note_id));

        return req;
//        return {};
    }

    std::optional<OpenNoteResponse> decodeOpenNoteResponse(const std::vector<uint8_t>& buffer) {
        constexpr size_t MIN_SIZE = 2 + 1 + 4 + 4 + 4; // op + status + note_id + title_len + text_len

        if (buffer.size() < MIN_SIZE) {
            return std::nullopt;
        }

        const uint8_t* ptr = buffer.data();

        // Проверяем операцию
        uint16_t op;
        std::memcpy(&op, ptr, sizeof(op));
        ptr += sizeof(op);

        if (op != static_cast<uint16_t>(Operation::OPEN_NOTE)) {
            return std::nullopt;
        }

        OpenNoteResponse resp;
        resp.op = Operation::OPEN_NOTE;

        // Статус
        std::memcpy(&resp.status, ptr, sizeof(resp.status));
        ptr += sizeof(resp.status);

        // ID заметки
        std::memcpy(&resp.note_id, ptr, sizeof(resp.note_id));
        ptr += sizeof(resp.note_id);

        // Длина заголовка
        uint32_t title_len;
        std::memcpy(&title_len, ptr, sizeof(title_len));
        ptr += sizeof(title_len);

        // Длина текста
        uint32_t text_len;
        std::memcpy(&text_len, ptr, sizeof(text_len));
        ptr += sizeof(text_len);

        // Проверяем общий размер
        size_t expected_size = MIN_SIZE + title_len + text_len;
        if (buffer.size() < expected_size) {
            return std::nullopt;
        }

        // Читаем заголовок
        if (title_len > 0) {
            resp.title.assign(reinterpret_cast<const char*>(ptr), title_len);
            ptr += title_len;
        }

        // Читаем текст
        if (text_len > 0) {
            resp.text.assign(reinterpret_cast<const char*>(ptr), text_len);
        }

        return resp;
//        return {};
    }


    std::vector<uint8_t> encodeUpdateTextRequest(const UpdateTextRequest& req) {
        uint32_t text_len = static_cast<uint32_t>(req.text.size());

        // 2(op) + 4(user_id) + 4(note_id) + 4(text_len) + text
        std::vector<uint8_t> buffer(2 + 4 + 4 + 4 + text_len);
        uint8_t* ptr = buffer.data();

        // Код операции
        uint16_t op = static_cast<uint16_t>(Operation::UPDATE_TEXT);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        // user_id
        std::memcpy(ptr, &req.user_id, sizeof(req.user_id));
        ptr += sizeof(req.user_id);

//        uint32_t user_id_net = htonl(req.user_id);
//        std::memcpy(ptr, &user_id_net, sizeof(user_id_net));
//        ptr += sizeof(user_id_net);

        // note_id
        std::memcpy(ptr, &req.note_id, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        // Длина текста
        std::memcpy(ptr, &text_len, sizeof(text_len));
        ptr += sizeof(text_len);

        // Текст заметки
        if (text_len > 0) {
            std::memcpy(ptr, req.text.data(), text_len);
        }

        return buffer;
//        return {};
    }

    std::vector<uint8_t> encodeUpdateTextResponse(const UpdateTextResponse& resp) {
        std::vector<uint8_t> buffer(2 + 1 + 4);
        uint8_t* ptr = buffer.data();

        // Код операции
        uint16_t op = static_cast<uint16_t>(resp.op);
        std::memcpy(ptr, &op, sizeof(op));
        ptr += sizeof(op);

        // Статус
        std::memcpy(ptr, &resp.status, sizeof(resp.status));
        ptr += sizeof(resp.status);

        // ID заметки
        std::memcpy(ptr, &resp.note_id, sizeof(resp.note_id));

        return buffer;
//        return {};
    }

    std::optional<UpdateTextRequest> decodeUpdateTextRequest(const std::vector<uint8_t>& buffer) {
        constexpr size_t MIN_SIZE = 2 + 4 + 4 + 4; // op + user_id + note_id + text_len

        if (buffer.size() < MIN_SIZE) {
            return std::nullopt;
        }

        const uint8_t* ptr = buffer.data();

        // Проверяем операцию
        uint16_t op;
        std::memcpy(&op, ptr, sizeof(op));
        ptr += sizeof(op);

        if (op != static_cast<uint16_t>(Operation::UPDATE_TEXT)) {
            return std::nullopt;
        }

        UpdateTextRequest req;

        // user_id
        std::memcpy(&req.user_id, ptr, sizeof(req.user_id));
        ptr += sizeof(req.user_id);

        // note_id
        std::memcpy(&req.note_id, ptr, sizeof(req.note_id));
        ptr += sizeof(req.note_id);

        // Длина текста
        uint32_t text_len;
        std::memcpy(&text_len, ptr, sizeof(text_len));
        ptr += sizeof(text_len);

        // Проверяем размер
        if (buffer.size() < MIN_SIZE + text_len) {
            return std::nullopt;
        }

        // Читаем текст
        if (text_len > 0) {
            req.text.assign(reinterpret_cast<const char*>(ptr), text_len);
        }

        return req;
//        return {};
    }

    std::optional<UpdateTextResponse> decodeUpdateTextResponse(const std::vector<uint8_t>& buffer) {
        constexpr size_t REQUIRED_SIZE = 2 + 1 + 4; // op + status + note_id

        if (buffer.size() < REQUIRED_SIZE) {
            return std::nullopt;
        }

        const uint8_t* ptr = buffer.data();

        // Проверяем операцию
        uint16_t op;
        std::memcpy(&op, ptr, sizeof(op));
        ptr += sizeof(op);

        if (op != static_cast<uint16_t>(Operation::UPDATE_TEXT)) {
            return std::nullopt;
        }

        UpdateTextResponse resp;
        resp.op = Operation::UPDATE_TEXT;

        // Статус
        std::memcpy(&resp.status, ptr, sizeof(resp.status));
        ptr += sizeof(resp.status);

        // ID заметки
        std::memcpy(&resp.note_id, ptr, sizeof(resp.note_id));

        return resp;
//        return {};
    }



}
