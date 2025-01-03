//
// Created by Rambo.Liu on 2024/12/30.
//

#include "TaskMgr.h"

TaskMgr::TaskMgr() {
    _stop = false;
    if (!_thread) {
        _thread = std::unique_ptr<std::thread>(new std::thread(std::bind(&TaskMgr::run, this)));
    }
}

TaskMgr::~TaskMgr() {
    _stop = true;
    printf("16------- ~TaskMgr 被析构\n");
}

void TaskMgr::pushTask(TaskPack &&task) {
    if(!_stop) {
        _taskQueue.push(std::move(task));
    }

}

bool TaskMgr::isStop() {
    return _stop;
}

void TaskMgr::join() {
    if (_thread && _thread->joinable()) {
        _thread->join();
    }
}

void TaskMgr::run() {
    while(!_stop) {
        TaskPack pack;
        _taskQueue.waitPop(pack);
        pack();
    }
    printf("46-----任务队列执行结束\n");
}

void TaskMgr::stop() {
    _stop = true;
    _taskQueue.stop();
}



