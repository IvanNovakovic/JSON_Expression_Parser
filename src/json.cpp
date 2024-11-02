// src/json.cpp
#include "../include/json_parser/json.hpp"

std::ostream& operator<<(std::ostream& os, const Json& json) {
    std::visit([&os](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            os << "null";
        } else if constexpr (std::is_same_v<T, bool>) {
            os << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int64_t>) {
            os << val;
        } else if constexpr (std::is_same_v<T, double>) {
            os << val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            os << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, Json>>) {
            os << "{";
            for (auto it = val.begin(); it != val.end(); ++it) {
                if (it != val.begin()) {
                    os << ", ";
                }
                os << "\"" << it->first << "\": " << it->second;
            }
            os << "}";
        } else if constexpr (std::is_same_v<T, std::vector<Json>>) {
            os << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i != 0) {
                    os << ", ";
                }
                os << val[i];
            }
            os << "]";
        }
    }, json.value);
    return os;
}