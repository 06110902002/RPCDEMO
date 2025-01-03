//
// Created by Rambo.Liu on 2024/12/20.
// Task 任务处理类

#ifndef RPCSERVER_TASKHANDLER_H
#define RPCSERVER_TASKHANDLER_H

#include <iostream>
#include "../meta.h"

/**
 * 任务回调处理类
 * @param R       回调函数处理的返回类型
 * @tparam F      回调处理函数
 * @tparam Args   参数列表
 */
template<typename R, typename ... Args>
class TaskHandler {
public:
    TaskHandler() {

    }

    ~TaskHandler() {
        printf("~TaskHandler 被析构:\n");
        callBack = nullptr;
        delArgs();
    }

    /**
     * 调用类成员变量函数
     * @tparam Func   类成员函数
     * @tparam Self   类
     * @param f
     * @param self
     * @param args    形参列表
     */
    template<typename Func, typename Self>
    void callMethodWithCls(const Func &f, Self *self,Args &&...args) {
        argsTuple = std::forward_as_tuple(args...);
        call_member_helper(f, self,
                           std::make_index_sequence<sizeof...(args)>{},
                           argsTuple);
    }

    /**
     * 使用std::function 包装回调函数
     * @param function  回调函数
     * @param args      形参列表
     */
    void setTargetAndArgs(std::function<R(Args...)> function, Args &&...args) {
        argsTuple = std::forward_as_tuple(args...);
        callBack = function;
    }

    std::function<R(Args...)> getTarget() {
        return callBack;
    }

    std::tuple<Args...> getArgs() {
        return argsTuple;
    }

    void run() {
        if (callBack) {
            callMethod(callBack,argsTuple);
        }
    }

    void delArgs() {
        std::cout << "删除参数列表中的内存" << std::endl;

        travel_tuple(argsTuple, [](auto item) {

            if constexpr (std::is_pointer<decltype(item)>::value) {
                std::cout << item << " 指针类型将删除" << std::endl;
                //delete item;
            } else {
                std::cout << item << " 非指针类型不处理" << std::endl;
            }
        });
        std::cout << std::endl;
    }



private:
    std::function<R(Args...)> callBack;
    //将参数保存在tuple 中，如果遇到 堆上的内存需要自己释放内存占用
    std::tuple<Args...> argsTuple;

};

#endif //RPCSERVER_TASKHANDLER_H
