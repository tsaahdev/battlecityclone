#pragma once

#include "common.h"

#include <algorithm>
#include <string>

namespace base {

static const char NULL_CHARACTER{ '\0' };
static char INVALID_CHARACTER{ '\0' };

template <class T>
size_t e_strlen(const T* str) {
    if (!str) { return 0; }
    auto end = str;
    while (*end) { ++end; }
    return end - str;
}

class String {
public:
    String(): data_{ static_cast<u8*>(malloc(1)) } {}
    String(const u8* nullTerminatedString) {
        if (!nullTerminatedString) {
            data_ = static_cast<u8*>(malloc(1));
            data_[0] = NULL_CHARACTER;
        } else {
            size_ = e_strlen(nullTerminatedString);
            data_ = static_cast<u8*>(malloc(size_ + 1));
            memcpy(data_, nullTerminatedString, size_);
            data_[size_] = NULL_CHARACTER;
        }
    }
    String(const char* nullTerminatedString): String(reinterpret_cast<const u8*>(nullTerminatedString)) {}
    String(const u8* nullTerminatedString, size_t size) {
        if (!nullTerminatedString) {
            data_ = static_cast<u8*>(malloc(1));
            data_[0] = NULL_CHARACTER;
        } else {
            size_ = std::min(e_strlen(nullTerminatedString), size);
            data_ = static_cast<u8*>(malloc(size_ + 1));
            memcpy(data_, nullTerminatedString, size_);
            data_[size_] = NULL_CHARACTER;
        }
    }
    String(const char* nullTerminatedString, size_t size): String{ reinterpret_cast<const u8*>(nullTerminatedString), size } {}
    String(const String& other)
        : data_{ static_cast<u8*>(malloc(other.size_ + 1)) }
        , size_{ other.size_ }
    {
        memcpy(data_, other.data_, size_);
    }
    String(String&& other)
        : data_{ other.data_ }
        , size_{ other.size_ }
    {
        other.size_ = 0;
        other.data_ = nullptr;
    }
    ~String() {
        free(data_);
    }

    String& operator=(const String& other) {
        if (this != &other) {
            String temp{ other };
            swap(temp);
        }
        return *this;
    }
    String& operator=(String&& other) {
        if (this != &other) {
            free(data_);
            size_ = other.size_;
            data_ = other.data_;
            other.size_ = 0;
            other.data_ = nullptr;
        }
        return *this;
    }
    void swap(String& other) {
        std::swap(size_, other.size_);
        std::swap(data_, other.data_);
    }
    size_t size() const { return size_; }
    b8 isEmpty() const { return size_ == 0; }
    char& operator[](size_t index) {
        if (index >= size_) {
            E_PRINT("index is out of range");
            INVALID_CHARACTER = NULL_CHARACTER;
            return INVALID_CHARACTER;
        }
        return reinterpret_cast<char*>(data_)[index];
    }
    const char& operator[](size_t index) const {
        if (index >= size_) {
            E_PRINT("index is out of range");
            return NULL_CHARACTER;
        }
        return reinterpret_cast<char*>(data_)[index];
    }
    const u8* data() const { return data_; }
    u8* data() { return data_; }
    const char* c_str() const {
        return reinterpret_cast<char*>(data_);
    }
    bool operator==(const String& other) {
        if (size_ != other.size_) { return false; }
        return std::equal(data_, data_ + size_, other.data_);
    }
    // bool operator!=(const String& other) const {
    //     return !(*this == other);
    // }
    String operator+(const String& other) const {
        String result;
        result.size_ = size_ + other.size_;
        result.data_ = static_cast<u8*>(malloc(result.size_ + 1));
        memcpy(result.data_, data_, size_);
        memcpy(result.data_ + size_, other.data_, other.size_);
        result.data_[result.size_] = NULL_CHARACTER;
        return result;
    }
    std::string toStdString() const { return std::string(reinterpret_cast<const char*>(data_), size_); }
private:
    u8* data_{ nullptr };
    size_t size_{ 0 };
};

class StringView {
public:
    StringView() = default;
    StringView(const StringView& other) = default;
    StringView(const u8* data, size_t size)
        : data_{ data }
        , size_{ size } {}
    StringView(const char* data, size_t size)
        : data_{ reinterpret_cast<const u8*>(data) }
        , size_{ size } {}
    StringView(const u8* data)
        : data_{ data }
        , size_{ data == nullptr ? 0 : e_strlen(data) } {}
    StringView(const char* data)
        : data_{ reinterpret_cast<const u8*>(data) }
        , size_{ data == nullptr ? 0 : e_strlen(data) } {}
    StringView& operator=(const StringView& other)  = default;
    size_t size() const { return size_; }
    b8 isEmpty() const { return size_ == 0; }
    const char& operator[](size_t index) const {
        if (index >= size_) {
            E_PRINT("index is out of range");
            return NULL_CHARACTER;
        }
        return reinterpret_cast<const char*>(data_)[index];
    }
    const char& at(size_t index) const {
        if (index >= size_) {
            E_PRINT("index is out of range");
            return NULL_CHARACTER;
        }
        return reinterpret_cast<const char*>(data_)[index];
    }
    const u8* data() const { return data_; }

    StringView substr(size_t pos, size_t length) const {
        if (pos >= size_) {
            E_PRINT("pos is out of range");
            return {};
        }
        const auto rcount = std::min(length, size_ - pos);
        return StringView(data_ + pos, rcount);
    }
    String toString() const { return String(data_, size_); }
    std::string toStdString() const { return std::string(reinterpret_cast<const char*>(data_), size_); }

private:
    const u8* data_{ nullptr };
    size_t size_{ 0 };
};

} // namespace base