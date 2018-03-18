//
// Created by ender on 3/17/18.
//
#ifndef ASYNCLOGGER_LOGGER_HPP
#define ASYNCLOGGER_LOGGER_HPP
#include <memory>
#include "LogStream.hpp"

typedef void (*OutputFunc)(const char* msg, int len);
typedef void (*FlushFunc)();

class SourceFile {
public:
    template<int N>
    inline SourceFile(const char (&arr)[N])
            : data_(arr),
              size_(N-1)
    {
        const char* slash = strrchr(data_, '/'); // builtin function
        if (slash)
        {
            data_ = slash + 1;
            size_ -= static_cast<int>(data_ - arr);
        }
    }

    explicit SourceFile(const char* filename)
            : data_(filename)
    {
        const char* slash = strrchr(filename, '/');
        if (slash)
        {
            data_ = slash + 1;
        }
        size_ = static_cast<int>(strlen(data_));
    }

    const char* data_;
    int size_;
};

/***** for user interface *****/
class Logger
{
public:
    enum LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };
    virtual ~Logger() {}

    static std::unique_ptr<Logger> createLogger(SourceFile file, int line);
    static std::unique_ptr<Logger> createLogger(SourceFile file, int line, LogLevel level);
    static std::unique_ptr<Logger> createLogger(SourceFile file, int line, LogLevel level, const char *func);

    // level
    virtual void setLogLevel(LogLevel level) = 0;
    static LogLevel logLevel() {
        return g_level;
    }

    static void setOutput(OutputFunc out) {
        Logger::putFunc_ = out;
    }

    // stream
    virtual LogStream& stream() = 0;

protected:
    Logger () {}

public:
    static  LogLevel    g_level;
    static  OutputFunc  putFunc_;
    static  FlushFunc   flushFunc_;
};

#define LOG_TRACE if (Logger::logLevel() <= Logger::TRACE) \
  Logger::createLogger(__FILE__, __LINE__, Logger::TRACE, __func__)->stream()
#define LOG_DEBUG if (Logger::logLevel() <= Logger::DEBUG) \
  Logger::createLogger(__FILE__, __LINE__, Logger::DEBUG, __func__)->stream()
#define LOG_INFO if (Logger::logLevel() <= Logger::INFO) \
  Logger::createLogger(__FILE__, __LINE__)->stream()
#define LOG_WARN Logger::createLogger(__FILE__, __LINE__, Logger::WARN)->stream()
#define LOG_ERROR Logger::createLogger(__FILE__, __LINE__, Logger::ERROR)->stream()
#define LOG_FATAL Logger::createLogger(__FILE__, __LINE__, Logger::FATAL)->stream()

#endif //ASYNCLOGGER_LOGGER_HPP
