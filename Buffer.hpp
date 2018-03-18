//
// Created by ender on 3/17/18.
//
#ifndef ASYNCLOGGER_BUFFER_HPP
#define ASYNCLOGGER_BUFFER_HPP

#include <boost/core/noncopyable.hpp>
#include <string>
#include <cstring>

using namespace std;
const int smallBuffer = 4000;
const int largeBuffer = 4000 * 1000;

template <int SIZE>
class FixedBuffer : boost::noncopyable
{
public:
    FixedBuffer() : cur_(data_) {}
    ~FixedBuffer() {}

    void append(const char * buf, size_t len) {
        if(available() > len) {
            std::memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    const char* data() const {return data_;}
    int length() const {return static_cast<int>(cur_ - data_);}

    bool empty() {return cur_ == data_;}
    char* current() {return cur_;}
    int available() const {return static_cast<int>(end() - cur_);}
    void add(size_t len) {cur_ += len;}

    std::string toString(){return std::string(data_, length());}

    // 可否替换成 + SIZE
    const char *end() const { return data_ + sizeof(data_);}
    char data_[SIZE];
    char *cur_;
};

typedef FixedBuffer<smallBuffer> Buffer;

#endif //ASYNCLOGGER_BUFFER_HPP
