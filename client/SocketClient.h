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
#include <memory>
#include <unordered_map>
#include "../bus/ByteUtils.h"

class SocketClient : public std::enable_shared_from_this<SocketClient> {

public:

    SocketClient() {
        _stop = false;
        _req_num = 0;
        _isConnected = false;
        _taskMgr = std::make_shared<TaskMgr>();
    }

    ~SocketClient() {
        _stop = true;
        _isConnected = true;
        printf("31-------- ~SocketClient 被析构\n");
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

    bool read() {
        while(!_stop) {
            uint8_t *type = ByteUtils::readLengthForClient(4, _socket_id);
            if (!type) {
                free(type);
                continue;
            }

            int nType = ByteUtils::byte2Int(type);

            //读取4 字节的长度
            uint8_t *length = ByteUtils::readLengthForClient(4, _socket_id);
            if (!length) {
                free(length);
                break;
            }

            int nlength = ByteUtils::byte2Int(length);

            uint8_t *content = ByteUtils::readLengthForClient(nlength, _socket_id);
            printf("收到信息内容：type = %d length = %d  content = %s \n", nType, nlength, content);

            std::string result(content,content + nlength);
            auto &f = _future_map[nType];
            if (f) {
                f->set_value(result);
            }
            //_result_promise.set_value(result);
        }
        return false;
    }


    bool sendMsg(int pro_type,const char* msg,size_t length) {
        if (pro_type <= 0 || !msg || length <= 0) {
            return false;
        }
        //发送4 字节 的报文类型
        uint8_t *type = ByteUtils::int2Bytes(pro_type);
        send(_socket_id, type, 4, 0);

        //发送报文长度
        uint8_t *frameLenght = ByteUtils::int2Bytes(length);
        send(_socket_id, frameLenght, 4, 0);

        //发送报文数据
        send(_socket_id, msg,length, 0);

        return true;
    }

    std::string sendMessage(std::string &msg) {
        CGRAPH_UNIQUE_LOCK lk(_conn_mtx);
        _conn_cond.wait(lk, [this] { return _isConnected; });
        auto p = std::make_shared<std::promise<std::string>>();
        std::future<std::string> result_future = p->get_future();//_result_promise.get_future();
        _req_num ++;
        _future_map.emplace(_req_num, std::move(p));
        sendMsg(_req_num, msg.c_str(), msg.length());
        std::string re = result_future.get();
        return re;

    }



    void start(std::string &ip, const int port) {
        std::packaged_task<bool()> task([this, ip, port]() {
            std::future<bool> connectFuture = commit(std::bind(&SocketClient::connectServer, this, ip, port));
            _isConnected = connectFuture.get();
            if (_isConnected) {
                _conn_cond.notify_one();
                read();
            }
            return _isConnected;
        });
        _connect_thread = std::make_unique<std::thread>(std::move(task));

    }

    void connectThreadJoin() {
        if (_connect_thread && _connect_thread->joinable()) {
            _connect_thread->join();
        }
    }

    void stop() {
        _stop = true;
    }


private:
    std::shared_ptr<TaskMgr>_taskMgr;
    int _socket_id;
    bool _stop;
    bool _isConnected;
    std::promise<std::string> _result_promise;
    int _req_num;
    std::mutex _conn_mtx;
    std::condition_variable _conn_cond;
    std::unique_ptr<std::thread>  _connect_thread;
    std::unordered_map<int, std::shared_ptr<std::promise<std::string>>> _future_map;
};

#endif //RPCSERVER_SOCKETCLIENT_H
