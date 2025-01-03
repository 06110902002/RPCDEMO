//
// Created by Rambo.Liu on 2024/9/13.
//

#include "NetServer.h"
#include "Socket.h"

NetServer::NetServer() {
    if (!_dispatcher) {
        _dispatcher = std::make_shared<Dispatcher>();
    }
}

NetServer::~NetServer() {
    printf("~NetServer  被析构\n");
}

void NetServer::setReadCallback(readCallBack &&cb) {
    _dispatcher->setReadCallback(std::move(cb));
}

void NetServer::setAcceptCallback(connectedStatusCallBack &&cb) {
   _dispatcher->setAcceptCallback(std::move(cb));
}

void NetServer::setWriteCallback(writeCallBack &&cb) {
    _dispatcher->setWriteCallback(std::move(cb));
}
bool NetServer::listenAndAccept(const std::string &ip, uint16_t port) {
    _dispatcher->bind(ip,port);
    _dispatcher->listenAndAccept();
//    _dispatcher->recv();
    _dispatcher->start();


}

void NetServer::stop() {
    _dispatcher->stop();
}

void NetServer::write(uint32_t type, uint32_t length, const char *data) {
    _dispatcher->write(type,length,data);
}
