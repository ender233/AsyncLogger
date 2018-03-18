//
// Created by ender on 3/17/18.
//
#include "Logger.hpp"
#include <thread>
#include <iomanip>

OutputFunc Logger::putFunc_ = NULL;
Logger::LogLevel Logger::g_level = Logger::INFO;

const char *LogLevelName[6] =
{
    "[TRACE] ",
    "[DEBUG] ",
    "[INFO]  ",
    "[WARN]  ",
    "[ERROR] ",
    "[FATAL] ",
};

inline LogStream& operator<<(LogStream& s, const SourceFile& v)
{
    s.append(v.data_, v.size_);
    return s;
}

class LoggerImpl : public Logger
{
public:
    LoggerImpl(LogLevel level, int error, const SourceFile& file, int line)
        :  stream_(),
           level_(level),
           line_(line),
           basename_(file)
    {
        auto t = chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto curTime = std::put_time(std::localtime(&t), "%Y-%m-%d %X");
        std::ostringstream os;
        os << curTime;
        auto time = os.str();
        stream_ << "[" << time.c_str() << "]";

        size_t tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
        stream_ << "[" << tid << "]";
        stream_.append(LogLevelName[level], 7);
    }

    ~LoggerImpl() {
        finish();
        const Buffer &buffer = stream().buffer();
        putFunc_(buffer.data(), buffer.length());
    }

    void setLogLevel(LogLevel level) {
        level_ = level;
    }

    LogStream &stream() {
        return stream_;
    }

private:
    void finish() {
        stream_ << " - " << basename_ << ":" << line_ << "\n";
    }

private:
    LogLevel    level_;
    LogStream   stream_;
    int         line_;
    SourceFile  basename_;
};

std::unique_ptr<Logger> Logger::createLogger(SourceFile file, int line) {
    return std::make_unique<LoggerImpl>(Logger::INFO, 0, file, line);
}

std::unique_ptr<Logger> Logger::createLogger(SourceFile file, int line, LogLevel level) {
    return std::make_unique<LoggerImpl>(level, 0, file, line);
}

std::unique_ptr<Logger> Logger::createLogger(SourceFile file, int line, LogLevel level, const char *func)
{
    std::unique_ptr<Logger> ptr = std::make_unique<LoggerImpl>(level, 0, file, line);
    ptr->stream() << func << ' ';
    return ptr;
}
