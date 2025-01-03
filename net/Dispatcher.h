//
// Created by Rambo.Liu on 2024/9/13.
// 任务分发器 用于处理socket 通道的各项任务

#ifndef AIRCONTROL_DISPATCHER_H
#define AIRCONTROL_DISPATCHER_H


#include "../server/Thread.h"
#include "../server/BaseTask.h"
#include "NetType.h"
#include "Socket.h"

class Dispatcher:  public std::enable_shared_from_this<Dispatcher> {

public:

    Dispatcher();
    ~Dispatcher() ;

    void setReadCallback(readCallBack &&cb);

    void setWriteCallback(writeCallBack &&cb);

    void setAcceptCallback(connectedStatusCallBack &&cb);

    bool bind(const std::string& ip, uint16_t port);

    bool listenAndAccept();

    bool recv();

    void start();

    void stop();

    void write(uint32_t type,uint32_t length,const char *data);


private:
    std::shared_ptr<Socket> _socket;
    std::queue<BaseTask<bool,std::string,uint16_t>*> _taskQueue;
};


#endif //AIRCONTROL_DISPATCHER_H
