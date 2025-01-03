//
// Created by Rambo.Liu on 2024/12/30.
// 任务包装器 包装具体的任务

#ifndef RPCSERVER_TASKPACK_H
#define RPCSERVER_TASKPACK_H

#include <type_traits>
#include "Object.h"
#include <iostream>
#include "ObjectUtils.h"

class TaskPack : public Object, std::enable_shared_from_this<TaskPack> {

    struct taskBased {
        explicit taskBased() = default;

        virtual void call() = 0;

        virtual ~taskBased() = default;
    };

    // 退化以获得实际类型，修改思路参考：https://github.com/ChunelFeng/CThreadPool/pull/3
    template<typename F, typename T = typename std::decay<F>::type>
    struct taskDerided : taskBased {
        T func_;

        explicit taskDerided(F &&func) : func_(std::forward<F>(func)) {}

        void call() override {
            func_();
        }
    };

public:
    template<typename F>
    TaskPack(F&& f, int priority = 0)
    : impl_(new taskDerided<F>(std::forward<F>(f)))
    , priority_(priority) {
        //printf("40------TaskPack(F&& f, int priority = 0)\n");
    }

    void operator()() {
        if (impl_) {
            impl_->call();
        } else {
            printf("47------impl_ is null\n");
        }
    }

    TaskPack()  {
        //printf("48--------\n");
    }

    /**
     * 移动构造函数 减少形参的拷贝次数
     * @param task
     */
    TaskPack(TaskPack&& task) noexcept:
    impl_(std::move(task.impl_)),
    priority_(task.priority_) {
        //printf("56------TaskPack(TaskPack&& task) noexcept\n");
    }

    TaskPack &operator=(TaskPack&& task) noexcept {
        impl_ = std::move(task.impl_);
        priority_ = task.priority_;
        return *this;
    }

    bool operator>(const TaskPack& task) const {
        return priority_ < task.priority_;    // 新加入的，放到后面
    }

    bool operator<(const TaskPack& task) const {
        return priority_ >= task.priority_;
    }

    //禁止使用拷贝构造
    CGRAPH_NO_ALLOWED_COPY(TaskPack)

    void run() override {

    }

    ~TaskPack() {
        printf("81-------~TaskPack被析构\n");
    }

private:
    std::unique_ptr<taskBased> impl_ = nullptr;
    int priority_ = 0;                                 // 任务的优先级信息



};

#endif //RPCSERVER_TASKPACK_H
