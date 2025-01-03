//
// Created by Rambo.Liu on 2024/12/19.
// 网络连接的任务基类，任务分为同步、异步两类
// 整体的设计思路为 将网络连接 的各个动作按任务处理，比如绑定、监听、连接均视为一个任务实体
// 每做一个动作创建一个任务丢到任务队列中进行处理，达到统一管理的目的

#ifndef RPCSERVER_BASETASK_H
#define RPCSERVER_BASETASK_H

#include <memory>
#include <iostream>
#include <queue>
#include "TaskHandler.h"

enum TaskType {
    SYNC = 0,   //同步任务
    ASYNC       //异步任务
};


template<typename R,typename ... Args>
class BaseTask {

public:

    BaseTask() {

    }

    virtual ~BaseTask() {
        printf("~BaseTask 任务被析构\n");
    }

    virtual TaskType getType()  {
        return SYNC;
    }



    /**
     * 设置任务的执行回调函数
     * @param f    任务的回调函数
     * @param args 任务回调函数的参数
     */
    virtual void setTargetFunc(const std::function<R(Args...)>& target, Args &&...args) {
        std::cout << "打印参数: " << std::endl;
        (printArg(args),...);
        std::cout << "打印结束----- " << std::endl;
        TaskHandler<R,Args...>* _callBack = new TaskHandler<R,Args...>();
        _callBack->setTargetAndArgs(target,std::forward<Args>(args)...);
        _taskQueue.push(_callBack);

//        std::function<R(Args...)> func = std::bind(target,std::forward<Args>(args)...);
//        _taskQueue.push(func);

        //target(std::forward<Args>(args)...);
    }


    /**
     * 执行任务，主要实现思路：
     * 将回调函数用与函数参数存储在TaskHandler 类中
     * 其中函数回调函数用std::function 存储，参数用std::tuple 存储
     * 执行std::function 时需要注意的点为：需要将std::tuple 中的参数展开
     */
    virtual void exe() {
        int size = _taskQueue.size();
        for (int i = 0; i < size; i ++) {
            TaskHandler<R,Args...>* _callBack = _taskQueue.front();
            _callBack->run();
            _taskQueue.pop();
            delete _callBack;
        }
    }

    virtual void cancelWithType()  {
        int size = _taskQueue.size();
        for (int i = 0; i < size; i ++) {

        }
    }

    virtual void cancelAll() {
        while(!_taskQueue.empty()) {
            TaskHandler<R,Args...>* _callBack = _taskQueue.front();
            _taskQueue.pop();
            delete _callBack;
        }
    }

    /**
     * 打印参数
     * @tparam T 参数类型
     * @param t 参数实参
     */
    template <typename T>
    void printArg(T t) {
        std::cout << "参数类型 = " << typeid(t).name() << " 参数值 = " << t << std::endl;
    }



protected:
    std::queue<TaskHandler<R,Args...>*> _taskQueue;
    std::string _ip;

};


#endif //RPCSERVER_BASETASK_H
