#include <iostream>
#include "Logger.hpp"
#include "AsyncLogger.hpp"
using namespace std::chrono_literals;

//typedef void (*OutputFunc)(const char* msg, int len);
AsyncLogger logger("log.txt");

void outputToStd(const char* msg, int len) {
    std::string str(msg, len);
    std::cout << str << std::endl;
}

void outputToLogger(const char* msg, int len) {
    logger.appendMsg(msg, len);
}

int main() {
    //Logger::setOutput(outputToStd);

    Logger::setOutput(outputToLogger);
    logger.run();

    for(int i=0; i<99; i++) {
        LOG_INFO << "hello world" << i;
    }

    std::this_thread::sleep_for(2s);
    return 0;
}