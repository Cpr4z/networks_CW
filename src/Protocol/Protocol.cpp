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
        if (buffer.size() < 6) {
            return std::nullopt;
        }

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
        constexpr size_t MIN_SIZE = 3;
        if (buffer.size() < MIN_SIZE) {
            return std::nullopt;
        }
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
            if (buffer.size() < MIN_SIZE + sizeof(uint32_t)) {
                return std::nullopt;
            }
            std::memcpy(&response.user_id, ptr, sizeof(uint32_t));
//            ptr += sizeof(uint32_t);
        }
        return response;
    }
}
