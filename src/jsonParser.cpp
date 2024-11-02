// src/jsonParser.cpp
#include "../include/json_parser/jsonParser.hpp"

void JsonParser::skipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.size() && std::isspace(content[pos])) {
        ++pos;
    }
}

Json JsonParser::parseValue(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    if (content[pos] == '{') {
        return parseObject(content, pos);
    } else if (content[pos] == '[') {
        return parseArray(content, pos);
    } else if (content[pos] == '"') {
        return Json(parseString(content, pos));
    } else if (std::isdigit(content[pos]) || content[pos] == '-') {
        return parseNumber(content, pos);
    } else if (content.substr(pos, 4) == "null") {
        pos += 4;
        return nullptr;
    } else if (content.substr(pos, 4) == "true") {
        pos += 4;
        return true;
    } else if (content.substr(pos, 5) == "false") {
        pos += 5;
        return false;
    } else {
        throw std::runtime_error("Invalid JSON value");
    }
}

Json JsonParser::parseObject(const std::string& content, size_t& pos) {
    ++pos;
    std::unordered_map<std::string, Json> map;

    while (true) {
        skipWhitespace(content, pos);
        std::string key = parseString(content, pos);

        skipWhitespace(content, pos);
        if (content[pos] != ':') {
            throw std::runtime_error("Invalid JSON value: Expected ':' after key in object");
        }
        ++pos;

        Json value = parseValue(content, pos);
        map[key] = value;

        skipWhitespace(content, pos);
        if (content[pos] != ',') {
            break;
        }
        ++pos;
    }

    if (content[pos] != '}') {
        throw std::runtime_error("Invalid JSON value: Expected '}' at end of object");
    }
    ++pos;

    return Json(map);
}

Json JsonParser::parseArray(const std::string& content, size_t& pos) {
    ++pos;
    std::vector<Json> vec;

    while (true) {
        skipWhitespace(content, pos);
        vec.push_back(parseValue(content, pos));

        skipWhitespace(content, pos);
        if (content[pos] != ',') {
            break;
        }
        ++pos;
    }

    if (content[pos] != ']') {
        throw std::runtime_error("Invalid JSON value: Expected ']' at end of array");
    }
    ++pos;

    return Json(vec);
}

std::string JsonParser::parseString(const std::string& content, size_t& pos) {
    if (content[pos] != '"') {
        throw std::runtime_error("Invalid JSON value: Expected '\"' at the beginning of string");
    }

    ++pos;
    std::string result;

    while (pos < content.size() && content[pos] != '"') {
        result += content[pos];
        ++pos;
    }

    if (content[pos] != '"') {
        throw std::runtime_error("Invalid JSON value: Expected '\"' at the end of string");
    }

    ++pos;

    return result;
}

Json JsonParser::parseNumber(const std::string& json, size_t& pos) {
    size_t start = pos;
    bool isDouble = false;

    while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '.' || json[pos] =='-')) {
        if (json[pos] == '.') {
            if (!isDouble) {
                isDouble = true;
            } else {
                throw std::runtime_error("Invalid JSON value: Number has more than one decimal point");
            }
        }
        ++pos;
    }

    if (isDouble) {
        return Json(std::stod(json.substr(start, pos - start)));
    } else {
        return Json(static_cast<int64_t>(std::stoi(json.substr(start, pos - start))));
    }
}