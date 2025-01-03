//
// Created by Rambo.Liu on 2024/9/13.
//

#ifndef AIRCONTROL_NETTYPE_H
#define AIRCONTROL_NETTYPE_H

#include <cstdint>
#include <functional>
#include <memory>

template <typename T>
struct SysCallResult {
    T _return_value;
    int32_t _errno;
};

using SysCallInt32Result = SysCallResult<int32_t>;
using SysCallInt64Result = SysCallResult<int64_t>;


// error code
enum NET_ERROR_CODE {
    SOCKET_CONNECTED                = 0,    // success.
    SOCKET_DISCONNECTED             = 1,    // remote close the socket.
    SOCKET_BIND_FAIL                = 2,    // bind fail
    SOCKET_INIT_ERROR               = 3,    // init error
    SOCKET_STOP_LISTENER            = 4,    // stop listener
};

/**
 * 连接状态回调
 */
using connectedStatusCallBack = std::function<void (int clientId, NET_ERROR_CODE status,const char *msg)>;

/**
 * 写数据回调
 */
using writeCallBack = std::function<void (int clientId, uint8_t*)>;

/**
 * 读数据回调
 * @param clientId 客户端连接的socketId
 * @param type     协议号
 * @param len      报文长度
 * @param data     数据内容
 */
using readCallBack = std::function<void (int clientId,int type, int len,uint8_t* data)>;



#endif //AIRCONTROL_NETTYPE_H
