#include "ConflictResolver.hpp"

#include <cctype>

namespace Protocol {

    static bool isAllWhitespace(const std::string& str) {
        if (str.empty()) return true;
        for (unsigned char c : str) {
            if (!std::isspace(c)) return false;
        }
        return true;
    }

    static bool containsNewline(const std::string& s) {
        return s.find('\n') != std::string::npos;
    }

    static bool canAutoMergeInsertsStrict(const std::string& ins1, const std::string& ins2) {
        const bool n1 = containsNewline(ins1);
        const bool n2 = containsNewline(ins2);

        if (isAllWhitespace(ins1) && isAllWhitespace(ins2)) return true;

        if (n1 != n2) return true;

        if (!n1 && !n2) return false;

        return false;
    }

    ConflictResolver::Edit ConflictResolver::diffToBase(const std::string& base,
                                                        const std::string& ver) {
        size_t prefix = 0;
        const size_t min_len = std::min(base.size(), ver.size());

        while (prefix < min_len && base[prefix] == ver[prefix]) {
            ++prefix;
        }

        size_t suffix = 0;
        while (suffix < min_len - prefix &&
               base[base.size() - 1 - suffix] == ver[ver.size() - 1 - suffix]) {
            ++suffix;
        }

        const size_t del_len = base.size() - prefix - suffix;
        const std::string ins = ver.substr(prefix, ver.size() - prefix - suffix);

        return { prefix, del_len, ins };
    }

    bool ConflictResolver::editsOverlap(const Edit& a, const Edit& b) {
        if (a.del_len == 0 && b.del_len == 0 && a.pos == b.pos) {
            return !canAutoMergeInsertsStrict(a.ins, b.ins);
        }

        const size_t aL = a.pos;
        const size_t aR = a.pos + a.del_len;
        const size_t bL = b.pos;
        const size_t bR = b.pos + b.del_len;

        if (a.del_len == 0 && b.del_len > 0) {
            return (aL >= bL && aL < bR);
        }
        if (b.del_len == 0 && a.del_len > 0) {
            return (bL >= aL && bL < aR);
        }

        return !(aR <= bL || bR <= aL);
    }

    void ConflictResolver::applyEdit(std::string& s, const Edit& e) {
        if (e.pos > s.size()) return;
        s.replace(e.pos, e.del_len, e.ins);
    }

    ConflictResolver::ConflictType ConflictResolver::detectConflictType(
            const std::string& base_version,
            const std::string& server_version,
            const std::string& local_version
    ) {
        if (server_version == local_version) return ConflictType::AutoMerge;
        if (server_version == base_version)  return ConflictType::AutoMerge;
        if (local_version == base_version)   return ConflictType::AutoMerge;

        const Edit eS = diffToBase(base_version, server_version);
        const Edit eL = diffToBase(base_version, local_version);

        return editsOverlap(eS, eL) ? ConflictType::Manual : ConflictType::AutoMerge;
    }

    std::optional<std::string> ConflictResolver::autoMergeIfNoConflict(
            const std::string& base_version,
            const std::string& server_version,
            const std::string& local_version,
            bool preferServerFirst
    ) {
        if (server_version == local_version) return server_version;
        if (server_version == base_version)  return local_version;
        if (local_version == base_version)   return server_version;

        if (detectConflictType(base_version, server_version, local_version) == ConflictType::Manual) {
            return std::nullopt;
        }

        const Edit eS = diffToBase(base_version, server_version);
        const Edit eL = diffToBase(base_version, local_version);

        if (eS.del_len == 0 && eL.del_len == 0 && eS.pos == eL.pos) {
            std::string merged = base_version;

            const bool sHasNl = containsNewline(eS.ins);
            const bool lHasNl = containsNewline(eL.ins);
            if (sHasNl != lHasNl) {
                if (!sHasNl) merged.insert(eS.pos, eS.ins + eL.ins);
                else         merged.insert(eS.pos, eL.ins + eS.ins);
                return merged;
            }

            if (preferServerFirst) merged.insert(eS.pos, eS.ins + eL.ins);
            else                   merged.insert(eS.pos, eL.ins + eS.ins);

            return merged;
        }

        std::string merged = base_version;

        if (eS.pos > eL.pos) {
            applyEdit(merged, eS);
            applyEdit(merged, eL);
        } else if (eL.pos > eS.pos) {
            applyEdit(merged, eL);
            applyEdit(merged, eS);
        } else {
            return std::nullopt;
        }

        return merged;
    }

}