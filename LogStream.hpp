//
// Created by ender on 3/17/18.
//
#ifndef ASYNCLOGGER_LOGSTREAM_HPP
#define ASYNCLOGGER_LOGSTREAM_HPP
#include "Buffer.hpp"
#include <algorithm>
#include <cstdio>

static const char digits[] = "9876543210123456789";
static const char* zero = digits + 9;
static_assert(sizeof(digits) == 20, "wrong number of digits");
static const char digitsHex[] = "0123456789ABCDEF";
static_assert(sizeof digitsHex == 17, "wrong number of digitsHex");

template<typename T>
size_t convert(char buf[], T value)
{
    T i = value;
    char* p = buf;
    do
    {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0)
    {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}
static size_t convertHex(char buf[], uintptr_t value)
{
    uintptr_t i = value;
    char* p = buf;

    do
    {
        int lsd = static_cast<int>(i % 16);
        i /= 16;
        *p++ = digitsHex[lsd];
    } while (i != 0);

    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}


class LogStream {
public:
    typedef LogStream self;
    LogStream() {}

    void append(const char* data, int len) {
        buffer_.append(data, len);
    }

    self& operator<<(const char* str)
    {
        if (str)
        {
            buffer_.append(str, strlen(str));
        }
        else
        {
            buffer_.append("(null)", 6);
        }
        return *this;
    }

    self& operator<<(bool v) {
        buffer_.append(v ? "1":"0", 1);
        return *this;
    }

    self&operator<<(int v) {
        formatInteger(v) ;
        return *this;
    }
    self& operator<<(long v) {
        formatInteger(v);
        return *this;
    }

    self& operator<<(short v) {
        *this << static_cast<int>(v);
        return *this;
    }
    self& operator<<(unsigned int v) {
        formatInteger<int>(v);
        return *this;
    }

    self&operator<<(unsigned long  v) {
        formatInteger(v);
        return *this;
    }

    self&operator<<(long long v) {
        formatInteger(v);
        return *this;
    }

    self&operator<<(unsigned long long v) {
        formatInteger(v);
        return *this;
    }

    // Hex
    self&operator<<(const void *p) {
        uintptr_t v = reinterpret_cast<uintptr_t >(p);
        if(buffer_.available() >= MaxSizeForInt) {
            char *buf = buffer_.current();
            buf[0] = '0';
            buf[1] = 'x';
            size_t len = convertHex(buf+2, v);
            buffer_.add(len+2);
        }
        return *this;
    }

    // double
    self&operator<<(double v) {
        if(buffer_.available() >= MaxSizeForInt) {
            int len = snprintf(buffer_.current(), MaxSizeForInt, "%.12g", v);
            buffer_.add(len);
        }
        return *this;
    }

    self&operator<<(float v) {
        *this << static_cast<double >(v);
        return *this;
    }

    const Buffer & buffer() const{
        return buffer_;
    }

private:
    template <typename T>
    void formatInteger(T v) {
        if(buffer_.available() >= MaxSizeForInt) {
            size_t len = convert(buffer_.current(), v);
            buffer_.add(len);
        }
    }

private:
    FixedBuffer<4000> buffer_;
    static const int MaxSizeForInt = 32;
};

#endif //ASYNCLOGGER_LOGSTREAM_HPP
