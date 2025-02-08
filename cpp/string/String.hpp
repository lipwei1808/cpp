#pragma once

#include <cstring>
#include <iterator>
#include <string>

#define SSO_SIZE 128

class String {
public:
    struct Iterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = char;
        using pointer = char*;
        using reference = char&;
        Iterator(pointer ptr): m_ptr(ptr) {}
        reference operator*() const { return *m_ptr; }
        pointer operator->() { return m_ptr; }

        Iterator& operator++() { m_ptr++; return *this; }
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
        Iterator& operator--() { m_ptr--; return *this; }
        Iterator operator--(int) { Iterator tmp = *this; --(*this); return tmp; }

        friend bool operator==(const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; }
        friend bool operator!=(const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; }
    private:
        pointer m_ptr;
    };
    String(); 
    String(const char* s, size_t len); 
    String(const char* s); 
    String(const String& str);
    String& operator=(const String& str); 
    String(String&& str);
    String& operator=(String&& str);

    String& append(const char* str); 
    String& append(const String& str); 
    String& operator+=(const char* str);
    String& operator+=(const String& str);
    char& operator[](size_t pos);
    
    size_t size() const;
    const char* c_str() const; 
    void clear(); 

    Iterator begin();
    Iterator end();

    bool operator==(const char* str) const; 
    bool operator==(const String& str) const; 

    std::string toStdString() const; 

    ~String(); 

    

private:
    void swap(String& str);

    char* ptr = nullptr;
    size_t len = 0;
    char smallBuffer[SSO_SIZE + 1];
    bool sso = true;
};
