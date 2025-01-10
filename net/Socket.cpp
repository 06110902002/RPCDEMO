//
// Created by Rambo.Liu on 2024/9/13.
//

#include "Socket.h"
#include "OsHandle.h"

Socket::Socket() {
    _timeout = {0,0};
    _sock = 0;
    _stop = true;
    memset(buff_,0,2048);
}

Socket::~Socket() {
    printf("17-----socket 被析构\n");
    stop();
    _sock = 0;
    _stop = true;
    _read_cb = nullptr;
    _write_cb = nullptr;
    _connect_cb = nullptr;
}

void Socket::setReadCallback(readCallBack &&cb) {
    _read_cb = cb;
}

void Socket::setWriteCallback(writeCallBack &&cb) {
    _write_cb = cb;
}

void Socket::setAcceptCallback(connectedStatusCallBack &&cb) {
    _connect_cb = cb;
}


bool Socket::listenerAndAccept(const std::string& ip, uint16_t port) {
    printf("执行 socket listenerAndAccept \n");
    auto ret = OsHandle::Listen(_sock);
    if (ret._return_value < 0) {
        printf("listen socket filed! error:%d, info:%d", ret._errno, ret._errno);
        OsHandle::Close(_sock);
        return false;
    }

    Address address;
    OsHandle::setConnectCallBack(std::move(_connect_cb));
    while(!_stop) {
        SysCallInt64Result result = OsHandle::Accept(_sock, address);
        _socketId = result._return_value;
        recv(ip,port);
        usleep(1 * 1000 * 1000);
    }

}

void Socket::stop() {
    _stop = true;
    OsHandle::Close(_sock);
}

bool Socket::bind(const std::string &ip, uint16_t port) {
    printf("执行 socket bind \n");
    if (_sock == 0) {
        auto ret = OsHandle::TcpSocket(Address::IsIpv4(ip));
        if (ret._return_value < 0) {
            printf("create socket failed. errno:%d, info:%d", ret._errno, ret._errno);
            return false;
        }
        _sock = ret._return_value;
    }
    _addr.SetType(Address::IsIpv4(ip) ? AT_IPV4 : AT_IPV6);
    _addr.SetIp(ip);
    _addr.SetAddrPort(port);

    auto ret = OsHandle::Bind(_sock, _addr);
    if (ret._return_value < 0 ) {
        printf("bind socket filed! error:%d, info:%d", ret._errno, ret._errno);
        OsHandle::Close(_sock);
        return false;
    }
    _stop = false;
    return true;
}

bool Socket::recv(const std::string &ip, uint16_t port) {
    printf("执行socket recv\n");

    while(!_stop) {
        uint8_t *type = readLengthForClient(4, _socketId);
        if (!type) {
            free(type);
            break;
        }

        int nType = byte2Int(type);

        //读取4 字节的长度
        uint8_t *length = readLengthForClient(4, _socketId);
        if (!length) {
            free(length);
            break;
        }

        int nlength = byte2Int(length);

        uint8_t *content = readLengthForClient(nlength, _socketId);

        if (_read_cb) {
            _read_cb(_socketId,nType, nlength, content);
        }
        printf("收到信息内容：type = %d length = %d  content = %s \n", nType, nlength, content);
        //char* msg = "server echo msg";
        //write(nType, strlen(msg),msg);
    }
    return false;
}

uint8_t *Socket::readLengthForClient(int length, int clientfd) {
    if (length <= 0 || clientfd < 0) {
        return nullptr;
    }
    uint8_t *buf = (uint8_t *) malloc(length * sizeof(uint8_t));
    memset(buf, 0, length);
    int read = 0;
    while (read < length) {
        int cur = 0;
        //LOGV( "总长度 = %d  已经读取  = %d",length, read);
        cur = OsHandle::Recv(clientfd, buf, length - read, 0);
        if (cur <= 0) {
            if (_connect_cb) {
                _connect_cb(clientfd,SOCKET_DISCONNECTED,"断开连接");
            }
            free(buf);
            buf = nullptr;
            break;
        }
        read += cur;
    }
    return buf;
}

void Socket::write(uint32_t type, uint32_t length, const char *data) {
    printf("；回传的数据 type = %d  legnth = %d  data = %s \n",type,length,data);
    OsHandle::Write(_socketId, reinterpret_cast<const char *>(int2Bytes(type)), 4);
    OsHandle::Write(_socketId, reinterpret_cast<const char *>(int2Bytes(length)), 4);
    OsHandle::Write(_socketId, data, length);
}

uint8_t *Socket::int2Bytes(int i) {
    uint8_t *buf = (uint8_t *) malloc(4 * sizeof(uint8_t));
    memset(buf, 0, 4);
    buf[0] = (unsigned char) i;
    buf[1] = i >> 8;
    buf[2] = i >> 16;
    buf[3] = i >> 24;
    return buf;

}

long Socket::byte2long(uint8_t *bytes) {
    long value = ((bytes[0] & 0xFF)
                  | ((bytes[1] & 0xFF) << 8)
                  | ((bytes[2] & 0xFF) << 16)
                  | ((bytes[3] & 0xFF) << 24));
    return value;
}

int Socket::byte2Int(uint8_t *bytes) {
    int value;
    value = (int) ((bytes[0] & 0xFF)
                   | ((bytes[1] & 0xFF) << 8)
                   | ((bytes[2] & 0xFF) << 16)
                   | ((bytes[3] & 0xFF) << 24));
    return value;
}



