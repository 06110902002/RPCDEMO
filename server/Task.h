//
// Created by Rambo.Liu on 2024/12/19.
//

#ifndef RPCSERVER_TASK_H
#define RPCSERVER_TASK_H

#include "BaseTask.h"

template<typename R, typename ... Args>
class Task : public BaseTask<R,  Args...>, public std::enable_shared_from_this<Task<R,Args...>> {

public:

    Task() {

    }

    ~Task() {
        printf("~Task 任务被析构\n");
    }

    void setTargetFunc(const std::function<R(Args...)>& target, Args &&...args) override {
        BaseTask<R, Args...>::setTargetFunc(target, std::forward<Args>(args)...);
    }

    /**
     * 调用类成员函数
     * @tparam Func   类成员函数
     * @tparam Self   类
     * @param f
     * @param self
     * @param args    形参列表
     */
    template<typename Func, typename Self>
    void callMemberFunction(const Func &f, Self *self,Args &&...args) {
        std::tuple<Args...> argsTuple = std::forward_as_tuple(args...);
        call_member_helper(f, self,
                           std::make_index_sequence<sizeof...(args)>{},
                           argsTuple);
    }


    TaskType getType() override {
        return SYNC;
    }

    void exe() override {
        BaseTask<R, Args...>::exe();
    }

    void cancel() {

    }

};


#endif //RPCSERVER_TASK_H
