//
// Created by Rambo.Liu on 2024/12/30.
//

#ifndef RPCSERVER_OBJECT_H
#define RPCSERVER_OBJECT_H
#include <mutex>
#include <iostream>

class Object {
public:
    /**
     * 默认构造函数
     */
    explicit Object() = default;

    /**
     * 初始化函数
     */
    virtual void init() {

    }

    /**
     * 流程处理函数
     */
    virtual void run() = 0;

    /**
     * 释放函数
     */
    virtual void destroy() {
    }

    /**
     * 默认析构函数
     */
    virtual ~Object() = default;

protected:
    std::mutex mutex_;
    std::condition_variable cv_;
};

#endif //RPCSERVER_OBJECT_H
