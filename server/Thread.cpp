//
// Created by Rambo.Liu on 2024/9/13.
//

#include "Thread.h"

Thread::Thread() {
    _stop = true;
}

Thread::~Thread() {
    _stop = true;
    printf("~Thread 被析构\n");
}

void Thread::Start() {
    _stop = false;
    if (!_thread) {
        _thread = std::unique_ptr<std::thread>(new std::thread(std::bind(&Thread::Run, this)));
    }
}

void Thread::Stop() {
    _stop = true;
}

void Thread::Join() {
    if (_thread && _thread->joinable()) {
        _thread->join();
    }
}

bool Thread::IsStop() {
    return _stop;
}
