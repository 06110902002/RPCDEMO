//
// Created by Rambo.Liu on 2024/9/13.
// 此类直接对接系统层socket

#ifndef AIRCONTROL_OSHANDLE_H
#define AIRCONTROL_OSHANDLE_H


#include "NetType.h"
#include "Address.h"


struct Iovec {
    void*     _iov_base;      // starting address of buffer
    size_t    _iov_len;       // size of buffer
    Iovec(void* base, size_t len): _iov_base(base), _iov_len(len) {}
};

class OsHandle {

public:
    static SysCallInt64Result TcpSocket(bool ipv4 = false);

    static SysCallInt32Result Bind(int64_t sockfd, Address& addr);

    static SysCallInt32Result Listen(int64_t sockfd, uint32_t len = 0);

    static SysCallInt32Result Connect(int64_t sockfd, Address& addr);

    static SysCallInt32Result Close(int64_t sockfd);

    static SysCallInt64Result Accept(int64_t sockfd, Address& address);

    static void setConnectCallBack(connectedStatusCallBack &&cb);

    static SysCallInt32Result Write(int64_t sockfd, const char *data, uint32_t len);
    static SysCallInt32Result Writev(int64_t sockfd, Iovec *vec, uint32_t vec_len);

    static int Recv(int64_t sockfd, uint8_t *data, uint32_t len, uint16_t flag);
    static SysCallInt32Result Readv(int64_t sockfd, Iovec *vec, uint32_t vec_len);

    static connectedStatusCallBack _connect_cb;
    static readCallBack _read_cb;
    static writeCallBack  _write_cb;

};


#endif //AIRCONTROL_OSHANDLE_H
