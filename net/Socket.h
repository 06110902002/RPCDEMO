//
// Created by Rambo.Liu on 2024/9/13.
//

#ifndef AIRCONTROL_SOCKET_H
#define AIRCONTROL_SOCKET_H

#include <memory>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "NetType.h"
#include "Address.h"
#include <queue>

//class BaseTask;

class Socket : public std::enable_shared_from_this<Socket> {

public:

    Socket();

    ~Socket();

    void setReadCallback(readCallBack &&cb);

    void setWriteCallback(writeCallBack &&cb);

    void setAcceptCallback(connectedStatusCallBack &&cb);

    bool bind(const std::string &ip, uint16_t port);

    bool listenerAndAccept(const std::string &ip, uint16_t port);

    bool recv(const std::string &ip, uint16_t port);

    void write(uint32_t type,uint32_t length,const char *data);

    uint8_t *readLengthForClient(int length, int clientfd);

    void stop();

    char buff_[2048];

private:

    struct timeval _timeout;
    bool _stop;

    readCallBack _read_cb;
    writeCallBack _write_cb;
    connectedStatusCallBack _connect_cb;
    Address _addr;
    uint64_t _sock;
    int64_t _socketId;

    uint8_t *int2Bytes(int i);

    long byte2long(uint8_t *bytes);

    int byte2Int(uint8_t *bytes);
    //std::shared_ptr<WebSocket> _webSocket;


};


#endif //AIRCONTROL_SOCKET_H
