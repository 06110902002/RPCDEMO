//
// Created by Rambo.Liu on 2024/12/30.
// 任务管理器

#ifndef RPCSERVER_TASKMGR_H
#define RPCSERVER_TASKMGR_H

#include <iostream>
#include "AtomicQueue.h"
#include "TaskPack.h"
#include <thread>
#include <future>

class TaskMgr : public Object, std::enable_shared_from_this<TaskMgr> {

public:
    TaskMgr();

    ~TaskMgr();

    void pushTask(TaskPack &&task);

    void run() override;

    void stop();

    void join();

    bool isStop();

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

private:
    std::atomic_bool _stop;
    AtomicQueue<TaskPack> _taskQueue;
    std::unique_ptr<std::thread> _thread;
};


#endif //RPCSERVER_TASKMGR_H
