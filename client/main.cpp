//
// Created by Rambo.Liu on 2024/12/24.
//
#include <future>
#include "iostream"
#include <sstream>
#include "RPCClient.h"
#include "StringUtils.h"
#include "Request.h"
#include "SocketClient.h"
#include "TaskMgr.h"
#include "AtomicQueue.h"

int f(int x, int y)
{
    return x + y;
}


void task_thread()
{
//    std::packaged_task<int(int, int)> task(f);
//    std::future<int> result = task.get_future();
//    std::this_thread::sleep_for(std::chrono::seconds (3));
//    std::thread task_td(std::move(task), 2, 10);
//    //task_td.join();
//
//    std::cout << "task_thread:\t" << result.get() <<
//              std::endl;
    printf("25--------\n");
    std::packaged_task<std::string()> task([](){
        std::cout << "\n27----packaged tid = " << std::this_thread::get_id() << std::endl;
        std::string result = "this is test msg";
        std::this_thread::sleep_for(std::chrono::seconds (3));
        printf("30--------\n");
        return result;
    });
    std::future<std::string> f1 = task.get_future();
    std::thread(std::move(task)).detach();
    printf("34--------%s \n ",f1.get().c_str());
}


template<typename... Ts>
std::string tuple_to_string(const std::tuple<Ts...>& tuple) {
    std::ostringstream oss;
    std::string sep;
    oss << "[";
    ([&](const auto& value) {
        oss << sep << value;
        sep = ", ";
    }(std::get<Ts>(tuple)), ...);
    oss << "]";
    return oss.str();
}

class MyFunction {
public:
    std::string concat(std::string& str) const {
        printf("59-----------std = %s\n",str.c_str());
        return info_ + str;
    }

    static int multiply(int i, int j) {
        return i * j;
    }

private:
    std::string info_ = "MyFunction : ";
};

void testRTCClient() {
    std::string ip2 = "127.0.0.1";
    int port2 = 12581;
    std::shared_ptr<RPCClient> rpcClient = std::make_shared<RPCClient>();
    rpcClient->start(ip2,port2);
    std::string re = rpcClient->call<std::string>("add",1,2);
    std::string re2 = rpcClient->call<std::string>("test",122,2);
    printf("85----------收到 re = %s  re2 = %s\n",re.c_str(),re2.c_str());
    rpcClient->connectThreadJoin();
}




int main() {
//    testCV();
//    return 0;

    testRTCClient();
    return 0;


    //测试socket client
    SocketClient* socketClient = new SocketClient();
    MyFunction mf;
    std::string str = "5";
    auto r3 = socketClient->commit(std::bind(&MyFunction::concat, mf, str));    // 返回值可以是任意类型
    printf("76--------str = %s \n",r3.get().c_str());

    std::packaged_task<float()> sendTask([socketClient](){
        printf("87---------sendmessage \n");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::string test = "this is test message";
        std::string result = socketClient->sendMessage(test);
        printf("91-------------收到 result = %s\n",result.c_str());
        return 2.0f;
    }); // 包装函数
    std::future<float> send = sendTask.get_future();  // 获取 future
    std::thread t222(std::move(sendTask)); // 在线程上运行


    std::string ip = "127.0.0.1";
    int port = 12581;
    socketClient->start(ip,port);
    return 0;


    //测试TaskMgr
    TaskMgr* taskMgr = new TaskMgr();
    //taskMgr->pushTask(std::bind(&MyFunction::concat, mf, str));


    std::packaged_task<float()> task44([taskMgr,mf](){
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::string str = "5666";
        taskMgr->commit(std::bind(&MyFunction::concat, mf, str));
        printf("37------添加函数\n");
        return 2.0f;
    }); // 包装函数
    std::future<float> f1 = task44.get_future();  // 获取 future
    std::thread t(std::move(task44)); // 在线程上运行


    std::packaged_task<float()> task45([taskMgr,mf](){
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "145---- 停止任务 = " << std::this_thread::get_id() << std::endl;
        std::string str = "停止任务";
        taskMgr->commit(std::bind(&MyFunction::concat, mf, str));
        taskMgr->stop();

        return 2.0f;
    }); // 包装函数
    std::future<float> f12 = task45.get_future();  // 获取 future
    std::thread t2(std::move(task45)); // 在线程上运行





//    std::packaged_task<float()> task46([taskMgr,mf](){
//        std::this_thread::sleep_for(std::chrono::seconds(15));
//        printf("110------测试停止任务\n");
//        return 2.0f;
//    }); // 包装函数
//    std::future<float> f123 = task46.get_future();  // 获取 future
//    std::thread t3(std::move(task46)); // 在线程上运行
//    t3.join();




    t.join();
    printf("107---------t 线程执行完毕\n");
    taskMgr->join();


    delete taskMgr;
    printf("107---------taskMgr 线程执行完毕\n");


//    std::tuple<int, double, std::string> tup = std::make_tuple(1, 2.34, "hello");
//    std::string result = tuple_to_string(tup);
//    std::cout << "60----"<< result << std::endl;
//
//    Request<int, std::string, double,std::string>* req = new Request<int, std::string, double,std::string>();
//    std::string s = req->convert2JSONStr("add",1, "7.34", 7.3,"liu alibaba");
//    std::cout << "65----"<< s << std::endl;


    //task_thread();


//    RPCClient* rpcClient = new RPCClient();
//    std::string s2 = rpcClient->call<std::string>("add",2,3);
//    printf("60----s2 = %s\n",s2.c_str());
//
//    rpcClient->async_call("add",[](StatusCode code, std::string data){
//                              printf("49------data = %s\n",data.c_str());
//        },
//                          12);

//    StringUtils<std::string,float>* st = new StringUtils<std::string,float>();
//    std::string in = "3.0f";
//    float r4 = st->convert(in);
//    printf("47-----r4 = %f\n",r4);




    return 0;
}