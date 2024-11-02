// include/json_parser/json.hpp
#ifndef JSON_HPP
#define JSON_HPP

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <algorithm>
#include <iostream>

/**
 * @class Json
 * @brief Represents a JSON value that can be any valid JSON data type
 * 
 * This class uses std::variant to store different JSON value types including:
 *  null, bool, int(int64_t), double, string, Object(unordered_map<std::string, Json>), Array(vector<Json>)
 */
class Json {
    std::variant<
        std::nullptr_t,
        bool,
        int64_t,
        double,
        std::string,
        std::unordered_map<std::string, Json>,
        std::vector<Json>> value;

public:
    Json() = default;
    Json(Json const&) = default;
    Json(Json&&) = default;

    Json(std::nullptr_t) : value(nullptr) {}
    Json(bool b) : value(b) {}
    Json(int64_t i) : value(i) {}
    Json(double d) : value(d) {}
    Json(const std::string& s) : value(s) {}
    Json(std::string&& s) : value(std::move(s)) {}
    Json(const std::unordered_map<std::string, Json>& obj) : value(obj) {}
    Json(std::unordered_map<std::string, Json>&& obj) : value(std::move(obj)) {}
    Json(const std::vector<Json>& arr) : value(arr) {}
    Json(std::vector<Json>&& arr) : value(std::move(arr)) {}

    // Assignment operators
    Json& operator=(Json const&) = default;
    Json& operator=(Json&&) = default;

    // Comparison operators
    bool operator==(Json const& other) const {
        return value == other.value;
    }
    bool operator!=(Json const& other) const {
        return value != other.value;
    }

    // Emplace method
    template <class T, class... Args>
    void emplace(Args&&... args) {
        value.emplace<T>(std::forward<Args>(args)...);
    }

    // Method to check the index of the currently active element
    std::size_t index() const {
        return value.index();
    }

    // Accessors
    bool isNull() const { return std::holds_alternative<std::nullptr_t>(value); }
    bool isBool() const { return std::holds_alternative<bool>(value); }
    bool isInt() const { return std::holds_alternative<int64_t>(value); }
    bool isDouble() const { return std::holds_alternative<double>(value); }
    bool isString() const { return std::holds_alternative<std::string>(value); }
    bool isObject() const { return std::holds_alternative<std::unordered_map<std::string, Json>>(value); }
    bool isArray() const { return std::holds_alternative<std::vector<Json>>(value); }

    bool asBool() const { return std::get<bool>(value); }
    int64_t asInt() const { return std::get<int64_t>(value); }
    double asDouble() const { return std::get<double>(value); }
    const std::string& asString() const { return std::get<std::string>(value); }
    const std::unordered_map<std::string, Json>& asObject() const { return std::get<std::unordered_map<std::string, Json>>(value); }
    const std::vector<Json>& asArray() const { return std::get<std::vector<Json>>(value); }

    // Mutators
    void setNull() { value = nullptr; }
    void setBool(bool b) { value = b; }
    void setInt(int64_t i) { value = i; }
    void setDouble(double d) { value = d; }
    void setString(const std::string& s) { value = s; }
    void setObject(const std::unordered_map<std::string, Json>& obj) { value = obj; }
    void setArray(const std::vector<Json>& arr) { value = arr; }

    // Overload operator[] for object access
    Json& operator[](const std::string& key) {
        if (!isObject()) {
            throw std::runtime_error("Error: Json value is not an object");
        }
        return std::get<std::unordered_map<std::string, Json>>(value)[key];
    }

    // Overload operator[] for array access
    Json& operator[](size_t index) {
        if (!isArray()) {
            throw std::runtime_error("Error: Json value is not an array");
        }
        return std::get<std::vector<Json>>(value)[index];
    }

    friend std::ostream& operator<<(std::ostream& os, const Json& json);
};

#endif // JSON_HPP