#pragma once

#include <string>
#include <optional>

namespace Protocol {

    class ConflictResolver {
    public:
        enum class ConflictType { AutoMerge, Manual };

        static ConflictType detectConflictType(const std::string& base_version,
                                               const std::string& server_version,
                                               const std::string& local_version);

        static std::optional<std::string> autoMergeIfNoConflict(
                const std::string& base_version,
                const std::string& server_version,
                const std::string& local_version,
                bool preferServerFirst = true
        );

    private:
        struct Edit {
            size_t pos;
            size_t del_len;
            std::string ins;
        };

        static Edit diffToBase(const std::string& base, const std::string& ver);
        static bool editsOverlap(const Edit& a, const Edit& b);
        static void applyEdit(std::string& s, const Edit& e);
    };

}