//
// Created by ender on 3/18/18.
//
#ifndef ASYNCLOGGER_ASYNCLOGGER_HPP
#define ASYNCLOGGER_ASYNCLOGGER_HPP

#include "Buffer.hpp"
#include "SyncQueue.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <fstream>
#include <string>
#include <chrono>

using namespace std::chrono_literals;
class AsyncLogger
{
public:
    typedef std::unique_ptr<Buffer> BufferPtr;
    typedef std::unique_ptr<std::thread> threadPtr;
    typedef SyncQueue<BufferPtr>    BufferSyncQueue;
    typedef std::unique_ptr<BufferSyncQueue> SyncQueuePtr;

    AsyncLogger(std::string log_file, int interval=1):
        running_state_(false),
        //file_name_(log_file),
        timeInterval_(interval),
        current_buffer_(new Buffer),
        current_queue_(new BufferSyncQueue(10)),
        backup_queue_(new BufferSyncQueue(10))
    {
    }

    void run() {
        running_state_ = true;
        logConsumeThread_.reset(new std::thread([&](){
            this->appendLog();
        }));
        logConsumeThread_->detach();
    }

    void appendMsg(const char *msg, int len) {
        std::lock_guard<std::mutex> locker(mutex_);
        if(current_buffer_->available() > len) {
            current_buffer_->append(msg, len);
        }
        else {
            current_queue_->Put(std::move(current_buffer_));
            current_buffer_.reset(new Buffer);
            current_buffer_->append(msg, len);
            //cond_.notify_one();
        }
    }

    void appendLog() {
        while(running_state_) {
            std::this_thread::sleep_for(1s);
            {
                std::lock_guard<std::mutex> locker(mutex_);
                if (!current_buffer_->empty()) {
                    current_queue_->Put(std::move(current_buffer_));
                    current_buffer_.reset(new Buffer);
                }
                if(current_queue_->Size() == 0) continue;

                std::swap(current_queue_, backup_queue_);
                current_queue_.reset(new BufferSyncQueue(10));
            }

            std::string file_name_ = "log.txt";
            ofstream of(file_name_);
            for(int i=0; i<backup_queue_->Size(); i++) {
                const Buffer & buffer = *(*backup_queue_)[i];
                std::string conStr(buffer.data(), buffer.length());
                of << conStr;
            }
            of.close();
        }
    }

    ~AsyncLogger() {
        stop();
    }

private:
    void stop() {
        running_state_ = false;
    }

private:
    bool                        running_state_;
    int                         timeInterval_;
    std::mutex                  mutex_;
    std::condition_variable_any cond_;
    threadPtr                   logConsumeThread_;

    BufferPtr                   current_buffer_;
    SyncQueuePtr                current_queue_;
    SyncQueuePtr                backup_queue_;

    // incomplete type ??
    // std::string                 file_name_;
    FILE *                      file_;
};

#endif //ASYNCLOGGER_ASYNCLOGGER_HPP
