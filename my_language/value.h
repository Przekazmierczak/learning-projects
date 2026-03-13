#ifndef VALUE_H
#define VALUE_H

#include <variant>
#include <string>

struct Value {
    std::variant<std::monostate, int, bool, std::string> value;

    Value() = default;
    Value(int valInt) : value(valInt) {};
    Value(bool valBool) : value(valBool) {};
    Value(std::string valString) : value(valString) {};

    bool operator==(const Value& other) const {
        return value == other.value;
    }

    bool isNaN() const {
        return std::holds_alternative<std::monostate>(value);
    }

    bool isInt() const {
        return std::holds_alternative<int>(value);
    }

    bool isBool() const {
        return std::holds_alternative<bool>(value);
    }

    bool isString() const {
        return std::holds_alternative<std::string>(value);
    }

    int getInt() const {
        return std::get<int>(value);
    }

    bool getBool() const {
        return std::get<bool>(value);
    }

    std::string getString() const {
        return std::get<std::string>(value);
    }
};

#endif