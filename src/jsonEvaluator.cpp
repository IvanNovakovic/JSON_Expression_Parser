// src/jsonEvaluator.cpp
#include "../include/json_parser/jsonEvaluator.hpp"

Json JsonEvaluator::evaluateExpression(const Json& json, const std::string& exp, size_t& pos) {
    const Json* current = &json;

    while (pos < exp.length()) {
        size_t start = pos;
        while (pos < exp.length() && exp[pos] != '.' && exp[pos] != '[') {
            ++pos;
        }

        if (pos > start) {
            evaluateObject(current, exp, start, pos);
        }

        if (pos < exp.length()) {
            if (exp[pos] == '.') {
                ++pos;
            } else if (exp[pos] == '[') {
                ++pos;
                evaluateList(json, current, exp, pos);
            }
        }
    }

    return *current;
}

void JsonEvaluator::evaluateObject(const Json*& current, const std::string& exp, size_t& start, size_t& pos) {
    std::string key = exp.substr(start, pos - start);

    if (!key.empty()) {
        if (!current->isObject()) {
            throw std::runtime_error("Invalid path: Expected object at '" + key + "'");
        }

        auto it = current->asObject().find(key);
        if (it == current->asObject().end()) {
            throw std::runtime_error("Key '" + key + "' not found");
        }

        current = &it->second;
    }
}

void JsonEvaluator::evaluateList(const Json& json, const Json*& current, const std::string& exp, size_t& pos) {
    if (!current->isArray()) {
        throw std::runtime_error("Invalid path: Expected array access");
    }

    size_t start = pos;
    int bracketCount = 1;
    bool isExpression = false;

    while (pos < exp.length() && bracketCount != 0) {
        if (exp[pos] == '[') {
            bracketCount++;
            isExpression = true;
        } else if (exp[pos] == ']') {
            bracketCount--;
        } else if (!std::isdigit(exp[pos])) {
            isExpression = true;
        }
        ++pos;
    }

    if (pos > exp.length() || bracketCount != 0) {
        throw std::runtime_error("Missing closing bracket ]");
    }

    std::string indexStr = exp.substr(start, pos - start - 1);

    // Number index
    if (std::all_of(indexStr.begin(), indexStr.end(), ::isdigit)) {
        size_t index = std::stoull(indexStr);
        const auto& arr = current->asArray();
        if (index >= arr.size()) {
            throw std::runtime_error("Array index out of bounds: " + indexStr);
        }
        current = &arr[index];
    } else {
        // Expression index
        size_t exprPos = 0;
        Json indexResult = evaluateExpression(json, indexStr, exprPos);
        if (!indexResult.isInt()) {
            throw std::runtime_error("Array index expression must evaluate to a number");
        }
        size_t index = static_cast<size_t>(indexResult.asInt());
        const auto& arr = current->asArray();
        if (index >= arr.size()) {
            throw std::runtime_error("Array index out of bounds: " + std::to_string(index));
        }
        current = &arr[index];
    }

    // Array of array a[1][4]
    if (pos < exp.length() && exp[pos] == '[') {
        ++pos;
        evaluateList(json, current, exp, pos);
    }
}

Json JsonEvaluator::evaluateMin(const Json& json, const std::string& exp, size_t& pos) {
    std::vector<std::string> substrings;
    std::vector<double> results;
    std::vector<std::future<Json>> futures;
    
    // Parse substrings
    while (pos < exp.size()) {
        size_t start = pos;
        while (pos < exp.size() && exp[pos] != ',' && exp[pos] != ')') {
            ++pos;
        }

        if (pos >= exp.size()) {
            throw std::runtime_error("Expected ',' or ')' in function");
        }

        if (pos != start) {
            substrings.push_back(exp.substr(start, pos - start));
        }
        ++pos;
    }

    // Multithreaded evaluate each substring
    for (const auto& substr : substrings) {
        futures.push_back(std::async(std::launch::async, [&json, substr]() {
            size_t pos = 0;

            if (!substr.empty() && (std::isdigit(substr[0]) || substr[0] == '-' || substr[0] == '.')) {
                bool hasDecimalPoint = false;
                for (size_t i = 1; i < substr.size(); ++i) {
                    if (!std::isdigit(substr[i])) {
                        if (substr[i] == '.' && !hasDecimalPoint) {
                            hasDecimalPoint = true;
                        } else {
                            throw std::runtime_error("More than one decimal point '.'");
                        }
                    }
                }

                return Json(std::stod(substr));
            }

            return JsonEvaluator::evaluateExpression(json, substr, pos);
        }));
    }

    // Wait for all futures
    results.reserve(futures.size());
    for (auto& future : futures) {
        Json result = future.get();
        if (result.isArray()) {

            // Arrays
            for (const auto& item : result.asArray()) {
                if (item.isInt()) {
                    results.push_back(static_cast<double>(item.asInt()));
                } else if (item.isDouble()) {
                    results.push_back(item.asDouble());
                } else {
                    throw std::runtime_error("Array elements must be numeric");
                }
            }

        }else if (result.isInt()) {
            results.push_back(static_cast<double>(result.asInt()));
        } else if (result.isDouble()) {
            results.push_back(result.asDouble());
        } else {
            throw std::runtime_error("Expression must evaluate to a number");
        }
    }

    if (results.empty()) {
        throw std::runtime_error("min function requires at least one numeric value");
    }

    double minValue = *std::min_element(results.begin(), results.end());
    return Json(minValue);
}

Json JsonEvaluator::evaluateMax(const Json& json, const std::string& exp, size_t& pos) {
    std::vector<std::string> substrings;
    std::vector<double> results;
    std::vector<std::future<Json>> futures;
    
    // Parse substrings
    while (pos < exp.size()) {
        size_t start = pos;
        while (pos < exp.size() && exp[pos] != ',' && exp[pos] != ')') {
            ++pos;
        }

        if (pos >= exp.size()) {
            throw std::runtime_error("Expected ',' or ')' in function");
        }

        if (pos != start) {
            substrings.push_back(exp.substr(start, pos - start));
        }
        ++pos;
    }

    // Multithreaded evaluate each substring
    for (const auto& substr : substrings) {
        futures.push_back(std::async(std::launch::async, [&json, substr]() {
            size_t pos = 0;

            if (!substr.empty() && (std::isdigit(substr[0]) || substr[0] == '-' || substr[0] == '.')) {
                bool hasDecimalPoint = false;
                for (size_t i = 1; i < substr.size(); ++i) {
                    if (!std::isdigit(substr[i])) {
                        if (substr[i] == '.' && !hasDecimalPoint) {
                            hasDecimalPoint = true;
                        } else {
                            throw std::runtime_error("More than one decimal point '.'");
                        }
                    }
                }

                return Json(std::stod(substr));
            }

            return JsonEvaluator::evaluateExpression(json, substr, pos);
        }));
    }

    // Wait for all futures
    results.reserve(futures.size());
    for (auto& future : futures) {
        Json result = future.get();
        if (result.isArray()) {

            // Arrays
            for (const auto& item : result.asArray()) {
                if (item.isInt()) {
                    results.push_back(static_cast<double>(item.asInt()));
                } else if (item.isDouble()) {
                    results.push_back(item.asDouble());
                } else {
                    throw std::runtime_error("Array elements must be numeric");
                }
            }

        }else if (result.isInt()) {
            results.push_back(static_cast<double>(result.asInt()));
        } else if (result.isDouble()) {
            results.push_back(result.asDouble());
        } else {
            throw std::runtime_error("Expression must evaluate to a number");
        }
    }

    if (results.empty()) {
        throw std::runtime_error("max function requires at least one numeric value");
    }

    double maxValue = *std::max_element(results.begin(), results.end());
    return Json(maxValue);
}

Json JsonEvaluator::evaluateSize(const Json& json, const std::string& exp, size_t& pos) {
    size_t startArg = pos;
    int parenthesisCount = 1;

    while (pos < exp.size() && parenthesisCount > 0) {
        if (exp[pos] == '(') {
            ++parenthesisCount;
        } else if (exp[pos] == ')') {
            --parenthesisCount;
            if (parenthesisCount == 0) {
                break;
            }
        }
        ++pos;
    }

    if (pos >= exp.size() || exp[pos] != ')') {
        throw std::runtime_error("Expected closing parenthesis in size function");
    }

    std::string argExp = exp.substr(startArg, pos - startArg);
    ++pos;

    Json argValue = evaluate(json, argExp);

    int64_t size = 0;
    if (argValue.isString()) {
        size = static_cast<int64_t>(argValue.asString().length());
    } else if (argValue.isArray()) {
        size = static_cast<int64_t>(argValue.asArray().size());
    } else if (argValue.isObject()) {
        size = static_cast<int64_t>(argValue.asObject().size());
    } else {
        throw std::runtime_error("size function argument must be a string, array, or object");
    }

    return Json(size);
}