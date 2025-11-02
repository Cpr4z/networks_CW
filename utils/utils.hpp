#pragma once

#include <filesystem>
#include <variant>
#include <fstream>
#include <regex>

//#include <algorithm>

#include <BasicTypes.hpp>

using ConfigValue = std::variant<int, std::string>;
using ConfigMap = Map<std::string, ConfigValue>;

namespace Utils {
    inline std::string removeSpaces(const std::string& str) {
        return std::regex_replace(str, std::regex("\\s+"), "");
    }

    inline std::string trim(const std::string& str) {
        removeSpaces(str);
        size_t first_pos = str.find_first_not_of("\t\n\r");
        if (first_pos == std::string::npos) {
            return {};
        }
        size_t last_pos = str.find_last_not_of(" \t\n\r");
        return str.substr(first_pos, (last_pos - first_pos) + 1);
    }

    ConfigValue parseValue(const std::string& value) {
        ConfigValue result;
        try {
            int number_value = std::stoi(value);
            result = number_value;
            return result;
        } catch (...) {}
        result = value;
        return result;
    }

    ConfigMap readConfigParams(const std::filesystem::path& config_path) noexcept {
        std::ifstream os(config_path);
        if (os.is_open()) {
            ConfigMap config_map;
            std::string line;
            while(std::getline(os, line)) {
                std::string trimmed_line = trim(line);
                size_t delim_pos = trimmed_line.find(':');
                std::string key = trimmed_line.substr(0, delim_pos);
                std::string value = trimmed_line.substr(delim_pos, trimmed_line.size() - delim_pos);
                config_map[key] = parseValue(value);
            }
            return config_map;
        }
        return {};
    }
};
