//
// Created by Rambo.Liu on 2024/9/13.
//

#include "Dispatcher.h"
#include "../server/AsyncTask.h"
#include "../server/Task.h"

Dispatcher::Dispatcher() {
    if (!_socket) {
        _socket = std::make_shared<Socket>();
    }
}

Dispatcher::~Dispatcher() {
    printf("Dispatcher 被释放\n");
    while (!_taskQueue.empty()) {
        BaseTask<bool, std::string, uint16_t> *value = _taskQueue.front();
        value->cancelAll();
        _taskQueue.pop();
        delete value;
        value = nullptr;
    }
}

void Dispatcher::setReadCallback(readCallBack &&cb) {
    _socket->setReadCallback(std::move(cb));
}

void Dispatcher::setWriteCallback(writeCallBack &&cb) {
    _socket->setWriteCallback(std::move(cb));
}

void Dispatcher::setAcceptCallback(connectedStatusCallBack &&cb) {
    _socket->setAcceptCallback(std::move(cb));
}

bool Dispatcher::bind(const std::string &ip, uint16_t port) {
    BaseTask<bool, std::string, uint16_t> *bindTask = new Task<bool, std::string, uint16_t>();
    std::function<bool(const std::string, uint16_t)> bindFunc = std::bind(&Socket::bind, _socket.get(), ip, port);
    bindTask->setTargetFunc(bindFunc, std::decay_t<const std::string &>(ip), std::move(port));
    _taskQueue.push(bindTask);
    return false;
}

bool Dispatcher::listenAndAccept() {
    AsyncTask<bool, std::string, uint16_t>* acceptTask = new AsyncTask<bool, std::string, uint16_t>();
    std::function<bool(const std::string, uint16_t)> listenerAndAcceptFunc = std::bind(&Socket::listenerAndAccept,
                                                                                       _socket.get(), "ip", 100);
    acceptTask->setTargetFunc(listenerAndAcceptFunc,"ip", 100);
    _taskQueue.push(acceptTask);

    return false;
}

bool Dispatcher::recv() {
    AsyncTask<bool, std::string, uint16_t>* acceptTask = new AsyncTask<bool, std::string, uint16_t>();
    std::function<bool(const std::string,uint16_t)> recvFunc = std::bind(&Socket::recv, _socket.get(), "ip",100);
    acceptTask->setTargetFunc(recvFunc,"ip", 100);
    _taskQueue.push(acceptTask);
    return false;
}

void Dispatcher::start() {
    unsigned long size = _taskQueue.size();
    for (int i = 0; i < size; ++i) {
        BaseTask<bool, std::string, uint16_t> *task = _taskQueue.front();
        task->exe();
        _taskQueue.pop();
        if (i == size - 1 && task->getType() == ASYNC) {
            ((AsyncTask<bool, std::string, uint16_t> *) task)->Join();
        }
    }
}

void Dispatcher::stop() {
    _socket->stop();
    while (!_taskQueue.empty()) {
        BaseTask<bool, std::string, uint16_t>* value =  _taskQueue.front();
        value->cancelAll();
        _taskQueue.pop();
        delete value;
        value = nullptr;
    }
}

void Dispatcher::write(uint32_t type, uint32_t length, const char *data) {
    AsyncTask<void, uint32_t,uint32_t,const char*>* writeTask = new AsyncTask<void, uint32_t,uint32_t,const char*>();
    std::function<void(uint32_t,uint32_t,const char*)> recvFunc = std::bind(&Socket::write, _socket.get(), type,length,data);
    writeTask->setTargetFunc(recvFunc,std::move(type),std::move(length),std::move(data));
    writeTask->exe();
}

