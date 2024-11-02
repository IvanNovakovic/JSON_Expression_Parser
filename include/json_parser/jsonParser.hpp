// include/json_parser/jsonParser.hpp
#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include "json.hpp"
#include <string>
#include <stdexcept>
#include <regex>

/**
 * @class JsonParser
 * @brief Static function for parsing string to Json object
 */
class JsonParser {
public:
    static Json parse(const std::string& jsonString) {
        size_t pos = 0;
        return parseValue(jsonString, pos);
    }

private:
    static Json parseValue(const std::string& content, size_t& pos);
    static Json parseObject(const std::string& content, size_t& pos);
    static Json parseArray(const std::string& content, size_t& pos);
    static std::string parseString(const std::string& json, size_t& pos);
    static Json parseNumber(const std::string& json, size_t& pos);
    static void skipWhitespace(const std::string& content, size_t& pos);
};

#endif // JSON_PARSER_HPP