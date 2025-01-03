//
// Created by Rambo.Liu on 2024/12/30.
//

#ifndef RPCSERVER_SOCKETCLIENT_H
#define RPCSERVER_SOCKETCLIENT_H

#include <iostream>
#include <future>
#include "AtomicQueue.h"
#include "TaskMgr.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../bus/ByteUtils.h"

class SocketClient : public std::enable_shared_from_this<SocketClient> {

public:

    SocketClient() {
        _taskMgr = new TaskMgr();
    }

    ~SocketClient() {
        if (_taskMgr) {
            delete _taskMgr;
            _taskMgr = nullptr;
        }
    }


    /**
     * 提交任务信息,支持任意函数类型，且异步执行
     * 1、普通函数
     * 2、静态函数
     * 3、类成员函数
     * 4、类成员静态函数
     * @tparam FunctionType
     * @param func
     * @param index
     * @return
     */
    template<typename FunctionType>
    auto commit(const FunctionType &func)
    -> std::future<decltype(std::declval<FunctionType>()())> {
        using ResultType = decltype(std::declval<FunctionType>()());
        std::packaged_task<ResultType()> task(func);
        std::future<ResultType> result(task.get_future());
        pushTask(std::move(task));
        return result;
    }


    void pushTask(TaskPack &&task) {
        if (_taskMgr) {
            _taskMgr->pushTask(std::move(task));
        }
    }

    bool connectServer(std::string &ip, const int port) {

        ///定义sockfd
        _socket_id = socket(AF_INET, SOCK_STREAM, 0);

        ///定义sockaddr_in
        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        //服务器端口
        servaddr.sin_port = htons(port);
        //服务器ip，inet_addr用于IPv4的IP转换（十进制转换为二进制）
        //127.0.0.1是本地预留地址
        servaddr.sin_addr.s_addr = inet_addr(ip.c_str());

        //设置连接超时时间
        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        if (setsockopt(_socket_id, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
            std::cout << "Setting socket timeout failed\n";
            close(_socket_id);
            return false;
        }


        //连接服务器，成功返回0，错误返回-1
        if (connect(_socket_id, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
            perror("client connect error ");
            return false;
        }
        return true;
    }



    bool sendMsg(int pro_type,const char* msg,size_t length) {
        //发送4 字节 的报文类型
        uint8_t *type = ByteUtils::int2Bytes(pro_type);
        send(_socket_id, type, 4, 0);

        //发送报文长度
        uint8_t *frameLenght = ByteUtils::int2Bytes(length);
        send(_socket_id, frameLenght, 4, 0);

        //发送报文数据
        send(_socket_id, msg,length, 0);
    }



    void start(std::string &ip, const int port) {
        std::packaged_task<bool()> task([this, ip, port]() {
            std::future<bool> connectFuture = commit(std::bind(&SocketClient::connectServer, this, ip, port));
//            if (connectFuture.get()) {
//                const char* msg = "this is test msg 23";
//                size_t length = strlen(msg) + 1;
//                commit(std::bind(&SocketClient::sendMsg, this,1,msg,length));
//            }
            const char* msg = "this is test msg 23";
            size_t length = strlen(msg) + 1;
            commit(std::bind(&SocketClient::sendMsg, this,1,msg,length));
            return connectFuture.get();
        });
        std::future<bool> f1 = task.get_future();
        std::thread t(std::move(task));
        t.join();
        printf("99---------connected  = %d   t 线程执行完毕\n ", f1.get());
        _taskMgr->join();

    }

    void stop() {

    }


private:
    TaskMgr *_taskMgr;
    int _socket_id;
};

#endif //RPCSERVER_SOCKETCLIENT_H
