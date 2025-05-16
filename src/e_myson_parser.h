#pragma once

#include "common.h"

#include <unordered_map>
#include <vector>
#include <string>

namespace base::myson {

struct Value {
    StringView view;
    String toString() const { return view.toString(); }
    u32 toU32() const { return std::stoul(reinterpret_cast<const char*>(view.data())); }
    u64 toU64() const { return std::stoull(reinterpret_cast<const char*>(view.data())); }
    i32 toI32() const { return std::stol(reinterpret_cast<const char*>(view.data())); }
    i64 toI64() const { return std::stoll(reinterpret_cast<const char*>(view.data())); }
    f32 toF32() const { return std::stof(reinterpret_cast<const char*>(view.data())); }
    f64 toF64() const { return std::stod(reinterpret_cast<const char*>(view.data())); }
    b8 toBool() const { return view[0] == 't'; }
    std::vector<Value> values;
    std::unordered_map<std::string, Value> keyValues;
};

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
            while (view[pos] != '\'') {
                if (view[pos] == '\\') { ++pos; }
                if (view[pos] == '\0') { token.type = Token::INVALID; return token; }
                ++pos;
            }
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
            token.type = Token::NUMBER;
            token.view = view.substr(tokenStart, pos - tokenStart);
        } break;

        case 't': {
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
        }
    }
    return token;
}

Value parseValue(Token token, const StringView& view, size_t& pos);

Value parseObject(const StringView& view, size_t& pos) {
    Value object;
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

        object.keyValues.insert({ key.toStdString(), value });

        currentToken = nextToken(view, pos);

        if (currentToken.type == Token::COMMA) {
            currentToken = nextToken(view, pos);
        } else if (currentToken.type != Token::RCB) {
            E_PRINT("failed to find a comma or RCB");
            return {};
        }
    }
    return object;
}

Value parseArray(const StringView& view, size_t& pos) {
    Value array;
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
            return value;
        };
        default:
            E_PRINT("unexpected token");
            return {};
    }
    return {};
}

Value parseDocument(const StringView& view) {
    size_t pos = 0;
    Token currentToken = nextToken(view, pos);
    Value object;
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

        object.keyValues.insert({ key.toStdString(), value });

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
