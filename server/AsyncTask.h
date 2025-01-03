//
// Created by Rambo.Liu on 2024/12/23.
//

#ifndef RPCSERVER_ASYNCTASK_H
#define RPCSERVER_ASYNCTASK_H

#include "BaseTask.h"
#include "Thread.h"

template<typename R, typename ...Args>
class AsyncTask :
        public BaseTask<R,Args...>,
        public Thread,
        public std::enable_shared_from_this<AsyncTask<R, Args...>> {

public:

    AsyncTask() {
        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&cond, nullptr);
    }

    ~AsyncTask() {
        printf("~Async 被析构\n");
        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&mutex);
    }

    AsyncTask(const AsyncTask &) = delete;

    AsyncTask &operator=(const AsyncTask &) = delete;

    /**----------------extend BaseTask----------------*/
    void setTargetFunc(const std::function<R(Args...)>& target, Args &&...args) override {
        printf("存入执行对象AsyncTask\n");
        pthread_mutex_lock(&mutex);

        TaskHandler<R, Args...> *_callBack = new TaskHandler<R, Args...>();
        _callBack->setTargetAndArgs(target, std::forward<Args>(args)...);
        BaseTask<R, Args...>::_taskQueue.push(_callBack);

        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    TaskType getType() override {
        return ASYNC;
    }

    void exe() override {
        Start();
    }

    void cancelAll() override {
        Stop();
        int size = BaseTask<R, Args...>::_taskQueue.size();
        for (int i = 0; i < size; ++i) {
            TaskHandler<R,  Args...>* _callBack = BaseTask<R, Args...>::_taskQueue.front();
            delete _callBack;
        }
    }
    /**----------------extend BaseTask----------------*/


    //extend Thread
    void Run() override {
        _local_thread_id = std::this_thread::get_id();
        while (!IsStop()) {
            while (BaseTask<R,  Args...>::_taskQueue.empty()) {
                printf("待执行异步任务队列中无数据，将阻塞\n");
                struct timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                ts.tv_sec += 5;
                pthread_cond_timedwait(&cond, &mutex,&ts);
                Stop();
                break;
            }
            int size = BaseTask<R, Args...>::_taskQueue.size();
            for (int i = 0; i < size; ++i) {
                TaskHandler<R, Args...>* _callBack = BaseTask<R, Args...>::_taskQueue.front();
                _callBack->run();
                BaseTask<R, Args...>::_taskQueue.pop();
                delete _callBack;
            }
        }
    }

    void Start() override {
        Thread::Start();
    }

    void Stop() override {
        Thread::Stop();

    }

    void Join() override {
        Thread::Join();
    }

    bool IsStop() override {
        return Thread::IsStop();
    }

    std::thread::id GetThreadID() { return _local_thread_id; }


private:
    pthread_cond_t cond;
    pthread_mutex_t mutex;

};

#endif //RPCSERVER_ASYNCTASK_H
