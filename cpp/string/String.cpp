#include "Logger.hpp"
#include "String.hpp"

#include <algorithm>
#include <utility>
#include <cstring>

String::String(): len(0), sso(false) {
    LOG_DEBUG("Default constructor");
}

String::String(const char* s, size_t len) {
    LOG_DEBUG("Parameter char* constructor with len");
    if (s == nullptr) {
        return;
    }
    this->len = len;
    if (len <= SSO_SIZE) {
        std::memcpy(smallBuffer, s, len);
        smallBuffer[len] = '\0';
        sso = true;
        return;
    }
    sso = false;
    ptr = new char[len + 1];
    std::memcpy(ptr, s, len);
    ptr[len] = '\0';
}

String::String(const char* s) {
    LOG_DEBUG("Parameter char* constructor");
    if (s == nullptr) {
        return;
    }
    len = strlen(s);
    if (len <= SSO_SIZE) {
        std::memcpy(smallBuffer, s, len);
        smallBuffer[len] = '\0';
        sso = true;
        return;
    }
    sso = false;
    ptr = new char[len + 1];
    std::memcpy(ptr, s, len);
    ptr[len] = '\0';
}

String::String(const String& str): len(str.len), sso(str.sso) {
    LOG_DEBUG("Copy constructor");
    if (sso) {
        std::memcpy(smallBuffer, str.smallBuffer, len);
        smallBuffer[len] = '\0';
        return;
    }
    ptr = new char[len + 1];
    std::memcpy(ptr, str.ptr, len);
    ptr[len] = '\0';
}

String::String(String&& str): ptr(str.ptr), len(str.len), sso(str.sso) {
    LOG_DEBUG("Move constructor");
    if (sso) {
        std::memcpy(smallBuffer, str.smallBuffer, len);
        smallBuffer[len] = '\0';
        return;
    }
    str.ptr = nullptr;
}

String& String::operator=(const String& str) {
    LOG_DEBUG("Copy assignment operator");
    String s{str};
    swap(s);
    return *this;
}

String& String::operator=(String&& str) {
    LOG_DEBUG("Move assignment operator");
    String s{std::move(str)};
    swap(s);
    return *this;
}

String& String::append(const char* str) {
    if (str == nullptr) {
        return *this;
    }

    size_t sz = strlen(str);
    LOG_DEBUG("Append current len: %zu, append len: %zu", len, sz);
    if (sso) {
        if (sz + len <= SSO_SIZE) {
            std::memcpy(smallBuffer + len, str, sz);
            smallBuffer[len + sz] = '\0';
            len += sz;
            LOG_DEBUG("After append: %s", c_str());
            return *this;
        }
        sso = false;
    }

    char* existingPtr = sso ? smallBuffer : ptr;
    ptr = new char[len + sz + 1];
    memcpy(ptr, existingPtr, len);
    memcpy(ptr + len, str, sz);
    ptr[len + sz + 1] = '\0';
    len += sz;
    LOG_DEBUG("After append: %s", c_str());
    return *this;
}

String& String::append(const String& str) {
    return append(str.c_str());
}

String& String::operator+=(const char* str) {
    return append(str);
}
String& String::operator+=(const String& str) {
    return append(str);
}

char& String::operator[](size_t pos) {
    if (pos >= len) {
        pos = 0;
    }
    
    if (sso) {
        return smallBuffer[pos];
    }

    return ptr[pos];
}

size_t String::size() const {
    return this->len;
}

const char* String::c_str() const {
    // Handle case where smallBuffer could be garbage
    LOG_DEBUG("c_str len: %zu", len);
    if (len == 0) {
        return nullptr;
    }

    if (sso) {
        return smallBuffer;
    }
    return ptr;
}

void String::clear() {
    if (ptr) {
        delete ptr;
    }
    ptr = nullptr;
    len = 0;
}

String::Iterator String::begin() {
    char* buf = sso ? smallBuffer : ptr;
    return String::Iterator{buf};
}

String::Iterator String::end() {
    char* buf = sso ? smallBuffer : ptr;
    return String::Iterator{buf + len};
}
    
bool String::operator==(const char* str) const {
    size_t sz = strlen(str);
    LOG_DEBUG("Operator== char* str size: %zu, len: %zu", sz, len);
    if (sz != len) {
        return false;
    }

    const char* p = sso ? smallBuffer : ptr;
    return strncmp(p, str, len) == 0;
}

bool String::operator==(const String& str) const {
    LOG_DEBUG("Operator== str size: %zu, len: %zu", str.len, len);
    if (str.len != len) {
        return false;
    }

    const char* myPtr = sso ? smallBuffer : ptr;
    const char* otherPtr = str.sso ? str.smallBuffer : str.ptr;
    return strncmp(myPtr, otherPtr, len) == 0;
}

String::~String() {
    if (ptr) {
        delete ptr;
    }
}

void String::swap(String& str) {
    std::swap(len, str.len);
    std::swap(ptr, str.ptr);
    std::swap(sso, str.sso);
    std::swap(smallBuffer, str.smallBuffer);
}
