//
// Created by Rambo.Liu on 2024/9/13.
//

#ifndef AIRCONTROL_THREAD_H
#define AIRCONTROL_THREAD_H


#include <thread>     // about thread
#include <atomic>     // for atomic_bool
#include <memory>     // for shared_ptr
#include <functional> // for bind

class Thread {

public:

    Thread();

    virtual ~Thread();

    //base option
    virtual void Start();

    virtual void Stop();

    virtual void Join();

    //TO DO
    virtual void Run() = 0;

    virtual bool IsStop();

protected:
    Thread(const Thread &) = delete;

    Thread &operator=(const Thread &) = delete;

protected:
    std::atomic_bool _stop;
    std::unique_ptr<std::thread> _thread;
    std::thread::id _local_thread_id;
};


#endif //AIRCONTROL_THREAD_H
