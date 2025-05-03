#pragma once

#include "common.h"

#include <unordered_map>
#include <vector>
#include <string>

namespace base::myson {

// WTH am i doing?


// struct StringView {
//     u8* pointer{ nullptr };
//     size_t size{ 0 };
//     bool operator==(const StringView& other) const { return other.pointer == pointer && other.size == size; }
    
// };
// }

// namespace std {
//     template<>
//     struct hash<base::myson::StringView> {
//         size_t operator()(const base::myson::StringView& view) const {
//             return std::hash<size_t>{}(reinterpret_cast<size_t>(view.pointer)) ^ std::hash<size_t>{}(view.size);
//         }
//     };
// }


// namespace base::myson {
struct Array;
struct Object;
struct Value {
    virtual ~Value() = default;
    enum Type: u8 {
        T_Value,
        T_Object,
        T_Array
    } type{ T_Value };
    StringView view;
    String toString() const { return view.toString(); }
    u32 toU32() const { return std::stoul(reinterpret_cast<const char*>(view.data())); }
    u64 toU64() const { return std::stoull(reinterpret_cast<const char*>(view.data())); }
    i32 toI32() const { return std::stol(reinterpret_cast<const char*>(view.data())); }
    i64 toI64() const { return std::stoll(reinterpret_cast<const char*>(view.data())); }
    f32 toF32() const { return std::stof(reinterpret_cast<const char*>(view.data())); }
    f64 toF64() const { return std::stod(reinterpret_cast<const char*>(view.data())); }
    b8 toBool() const { return view[0] == 't'; }
    Object toObject() const;
    Array toArray() const;
};
struct Array: public Value {
    Array(): Value() {}
    std::vector<Value> values;
};
struct Object: public Value {
    Object(): Value() {}
    std::unordered_map<std::string, Value> values;
};

Array Value::toArray() const { return type == T_Array ? *dynamic_cast<const Array*>(this) : Array{}; }
Object Value::toObject() const { return type == T_Object ? *dynamic_cast<const Object*>(this) : Object{}; }

// String toString(const Value& value) { return {}; }
// u32 toU32(const Value& value) { return 0; }
// u64 toU64(const Value& value) { return 0; }
// b8 toBool(const Value& value) { return false; }
// Array toArray(const Value& value) { return {}; }
// Array toObject(const Value& value) { return {}; }

// size_t size(const Array& array) { return array.values.size(); }
// const Value& at(const Array& array, size_t index) { return array.values.at(index); }

// const Value& value(const Object& object, const String& key) { return object.values.at(key); }
// b8 contains(const Object& object, const String& key) { return object.values.contains(key); }

// looks like i want to write a parser
// do i need a reel tokenizer here?

// KEY, LCB, RCB, LSB, RSB, COLON, COMMA, STRING, NUMBER, BOOL, EOF

struct Token {
    enum Type: u8 {
        KEY, LCB, RCB, LSB, RSB, COLON, COMMA, STRING, NUMBER, BOOL, END, NUL, INVALID
    } type{ INVALID };
    StringView view;
};

void skipWhitespace(const StringView& view, size_t& pos) {
    while ((view.size() >= pos + 1) && (isspace(view[pos]) || view[pos] == '\r' || view[pos] == '\n')) { ++pos; }
}

template<class T, class U>
size_t e_strncmp(const T* s1, const U* s2, size_t n) {
    if (n == 0) { return 0; }
    while (n-- > 0) {
        if (*s1 != *s2) {
            return (*(u8*)(s1) - *(u8*)(s2));
        }
        if (*s1 =='\0') { return 0; }
        ++s1;
        ++s2;
    }
    return 0;
}


Token nextToken(const StringView& view, size_t& pos) {
    Token token;
    token.view = view.substr(pos, 1);
    skipWhitespace(view, pos);
    if (view.size() == pos) {
        token.type = Token::END;
        return token;
    }
    // token.view.pointer = &view.pointer[pos];
    // token.view.size = 1;

    auto tokenStart = pos;

    switch (view[pos]) {
        case '\0': token.type = Token::END; break;
        case '{': token.type = Token::LCB; ++pos; break;
        case '}': token.type = Token::RCB; ++pos; break;
        case '[': token.type = Token::LSB; ++pos; break;
        case ']': token.type = Token::RSB; ++pos; break;
        case ':': token.type = Token::COLON; ++pos; break;
        case ',': token.type = Token::COMMA; ++pos; break;
        case '\'': {
            ++pos;
            ++tokenStart;
            // token.view.pointer = &view.pointer[pos];
            while (view[pos] != '\'') {
                if (view[pos] == '\\') { ++pos; }
                if (view[pos] == '\0') { token.type = Token::INVALID; return token; }
                ++pos;
            }
            // token.view.size = &view.pointer[pos] - token.view.pointer;
            token.view = view.substr(tokenStart, pos - tokenStart);
            token.type = Token::STRING;
            ++pos;
        } break;

        case '-':
        case '+': {
            ++pos;
            ++tokenStart;
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            while (isdigit(view[pos])) { ++pos; }
            if (view[pos] == '.') { ++pos; while (isdigit(view[pos])) { ++pos; } }
            if (view[pos] == 'e' || view[pos] == 'E') {
                ++pos;
                if (view[pos] == '+' || view[pos] == '-') { ++pos; }
                while (isdigit(view[pos])) { ++pos; }
            }
            // token.view.size = &view.pointer[pos] - token.view.pointer;
            token.type = Token::NUMBER;
            token.view = view.substr(tokenStart, pos - tokenStart);
        } break;

        case 't': {
            // if (e_strncmp(&view.pointer[pos], "true", 4) == 0) {
            if (e_strncmp(view.data() + pos, "true", 4) == 0) {
                token.type = Token::BOOL;
                token.view = view.substr(tokenStart, 4);
                pos += 4;
                break;
            }
        }
        case 'f': {
            if (e_strncmp(view.data() + pos, "false", 5) == 0) {
                token.type = Token::BOOL;
                token.view = view.substr(tokenStart, 5);
                pos += 5;
                break;
            }
        }
        case 'n': {
            if (e_strncmp(view.data() + pos, "null", 4) == 0) {
                token.type = Token::NUL;
                token.view = view.substr(tokenStart, 4);
                pos += 4;
                break;
            }
        }
        default: {
            while (!isspace(view[pos]) && view[pos] != ':') { ++pos; }
            token.type = Token::KEY;
            token.view = view.substr(tokenStart, pos - tokenStart);
            // token.view.size = &view.pointer[pos] - token.view.pointer;
        }
    }
    return token;
}

Value parseValue(Token token, const StringView& view, size_t& pos);

Object parseObject(const StringView& view, size_t& pos) {
    Object object;
    object.type = Value::T_Object;
    // object.pointer = view.pointer;
    // object.size = view.size;
    Token currentToken = nextToken(view, pos);
    while (currentToken.type != Token::RCB) {
        if (currentToken.type != Token::KEY) {
            E_PRINT("failed to find a key in your object");
            return {};
        }
        StringView key = currentToken.view;
        currentToken = nextToken(view, pos);
        if (currentToken.type != Token::COLON) {
            E_PRINT("failed to find a colon after a key in your myson file");
            return {};
        }
        currentToken = nextToken(view, pos);
        Value value = parseValue(currentToken, view, pos);

        object.values.insert({ key.toStdString(), value });

        currentToken = nextToken(view, pos);

        if (currentToken.type == Token::COMMA) {
            currentToken = nextToken(view, pos);
        } else if (currentToken.type != Token::RCB) {
            E_PRINT("failed to find a comma or RCB");
            return {};
        }
    }
    // currentToken = nextToken(view, pos);
    return object;
}

Array parseArray(const StringView& view, size_t& pos) {
    Array array;
    array.type = Value::T_Array;
    // array.pointer = view.pointer;
    // array.size = view.size;
    Token currentToken = nextToken(view, pos);
    while (currentToken.type != Token::RSB) {
        Value value = parseValue(currentToken, view, pos);
        array.values.push_back(value);
        currentToken = nextToken(view, pos);
        if (currentToken.type == Token::COMMA) {
            currentToken = nextToken(view, pos);
        } else if (currentToken.type != Token::RSB) {
            E_PRINT("failed to find a comma or RSB");
            return {};
        }
    }
    // currentToken = nextToken(view, pos);
    return array;
}

Value parseValue(Token token, const StringView& view, size_t& pos) {
    switch (token.type) {
        case Token::LCB: return parseObject(view, pos);
        case Token::LSB: return parseArray(view, pos);
        case Token::STRING:
        case Token::NUMBER:
        case Token::BOOL:
        case Token::NUL: {
            Value value;
            value.view = token.view;
            // E_PRINT("parsed value %s", value.view.toStdString().c_str());
            // pos += token.view.size();
            return value;
        };

        // case Token::STRING: return parseString();
        // case Token::NUMBER: return parseNumber();
        // case Token::BOOL: return parseBool();
        // case Token::NUL: return parseNull();
        default:
            E_PRINT("unexpected token");
            return {};
    }
    return {};
}

Object parseDocument(const StringView& view) {
    size_t pos = 0;
    Token currentToken = nextToken(view, pos);
    Object object;
    object.type = Value::T_Object;
    // object.pointer = view.pointer;
    // object.size = view.size;
    while (currentToken.type != Token::END) {
        if (currentToken.type != Token::KEY) {
            E_PRINT("failed to find a key in your myson file");
            return {};
        }
        StringView key = currentToken.view;
        currentToken = nextToken(view, pos);
        if (currentToken.type != Token::COLON) {
            E_PRINT("failed to find a colon after a key in your myson file");
            return {};
        }
        currentToken = nextToken(view, pos);
        Value value = parseValue(currentToken, view, pos);

        object.values.insert({ key.toStdString(), value });

        currentToken = nextToken(view, pos);

        if (currentToken.type == Token::COMMA) {
            currentToken = nextToken(view, pos);
        } else if (currentToken.type != Token::END) {
            E_PRINT("failed to find a comma or EOF");
            return {};
        }
    }
    return object;
}




} // namespace base::myson
