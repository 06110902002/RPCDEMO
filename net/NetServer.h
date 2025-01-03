//
// Created by Rambo.Liu on 2024/9/13.
// 网络服务器 用于整个socket 的管理,网络接口由此统一暴露

#ifndef AIRCONTROL_NETSERVER_H
#define AIRCONTROL_NETSERVER_H

#include "NetType.h"
#include "Dispatcher.h"
class Socket;

class NetServer {

public:

    NetServer();

    ~NetServer();

    void setReadCallback(readCallBack &&cb);

    void setWriteCallback(writeCallBack &&cb);

    void setAcceptCallback(connectedStatusCallBack &&cb);

    bool listenAndAccept(const std::string &ip, uint16_t port);

    void write(uint32_t type,uint32_t length,const char *data);

    void stop();


private:
    std::shared_ptr<Dispatcher> _dispatcher;
};


#endif //AIRCONTROL_NETSERVER_H
