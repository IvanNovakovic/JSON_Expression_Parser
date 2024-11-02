// include/json_parser/jsonEvaluator.hpp
#ifndef JSON_EVALUATOR_HPP
#define JSON_EVALUATOR_HPP

#include "json.hpp"
#include <string>
#include <stdexcept>
#include <regex>
#include <algorithm>
#include <iostream>
#include <future>
#include <vector>


/**
 * @class JsonEvaluator
 * @brief Static function for evaluating a passed Json object with a passed string expression
 */
class JsonEvaluator {
public:
    static Json evaluate(const Json& json, const std::string& expression) {
        std::string exp = expression;
        exp.erase(std::remove_if(exp.begin(), exp.end(), ::isspace), exp.end());

        size_t pos = 0;

        if (exp.substr(0, 3) == "min" && exp[3] == '(') {
            pos += 4;
            return evaluateMin(json, exp, pos);
        } else if (exp.substr(0, 3) == "max" && exp[3] == '(') {
            pos += 4;
            return evaluateMax(json, exp, pos);
        } else if (exp.substr(0, 4) == "size" && exp[4] == '(') {
            pos += 5;
            return evaluateSize(json, exp, pos);
        } else {
            return evaluateExpression(json, exp, pos);
        }
    }

private:
    static Json evaluateExpression(const Json& json, const std::string& exp, size_t& pos);
    static void evaluateObject(const Json*& current, const std::string& exp, size_t& start, size_t& pos);
    static void evaluateList(const Json& json, const Json*& current, const std::string& exp, size_t& pos);

    static Json evaluateMin(const Json& json, const std::string& exp, size_t& pos);
    static Json evaluateMax(const Json& json, const std::string& exp, size_t& pos);
    static Json evaluateSize(const Json& json, const std::string& exp, size_t& pos);
};

#endif // JSON_EVALUATOR_HPP