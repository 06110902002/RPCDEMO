#include <iostream>
#include <sstream>
#include <map>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include "Task.h"
#include "AsyncTask.h"
#include "../meta.h"
#include "../net/NetServer.h"

// 使用第三方库实现序列化和反序列化
//#include <boost/serialization/serialization.hpp>
//#include <boost/serialization/map.hpp>

using namespace std;

#define PORT 12581

// 定义RPC函数类型
using RPCCallback = std::function<std::string(const std::string&)>;

class RPCHandler {
public:
    void registerCallback(const std::string& name, RPCCallback callback) {
        std::unique_lock<std::mutex> lock(mtx_);
        callbacks_[name] = callback;
    }

    std::string handleRequest(const std::string& request) {
        // 反序列化请求
//        std::map<std::string, std::string> requestMap;
//        std::istringstream is(request);
//        boost::archive::text_iarchive ia(is);
//        ia >> requestMap;

        // 查找并调用对应的回调函数
        std::string name = "add";
        std::string args = "1,2";
        std::unique_lock<std::mutex> lock(mtx_);
        auto it = callbacks_.find(name);
        if (it == callbacks_.end()) {
            return "Error: Unknown function";
        }
        RPCCallback callback = it->second;
        return callback(args);
    }

private:
    std::map<std::string, RPCCallback> callbacks_;
    std::mutex mtx_;
};

int add (char* ip,int port) {

    printf("52-----ip = %s  port = %d \n",ip,port);
    return 23;
}

void add2 (int a,int b,int c) {

    printf("57-----a = %d  b = %d  c = %d \n",a,b,c);
}



/**--------------------------遍历tuple 并将tuple 当作函数参数列表执行-----------------------------***/
template<typename Function, typename Tuple, std::size_t... Index>
decltype(auto) invoke_impl2(Function &&func, Tuple &&t, std::index_sequence<Index...>) {
    return func(std::get<Index>(std::forward<Tuple>(t))...);
}

template<typename Function, typename Tuple>
decltype(auto) invoke2(Function &&func, Tuple &&t) {
    constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
    return invoke_impl2(std::forward<Function>(func), std::forward<Tuple>(t), std::make_index_sequence<size>{});
}
/**--------------------------遍历tuple 并将tuple 当作函数参数列表执行-----------------------------***/



template<typename T, typename ...Args>
void setTargetFunc(const T &target, Args &&...args) {
    std::function<void(Args...)> func = std::bind(target,std::forward<Args>(args)...);
    func(std::forward<Args>(args)...);
}



template<typename R, typename ... Args>
class A {
public:
    A() {

    }

    template<typename T>
    void test(const T& r,Args&& ... args) {

    }

    template<typename Func, typename Self>
    void callMethod(const Func &f, Self *self,Args &&...args) {
        std::tuple<Args...> argsTuple = std::forward_as_tuple(args...);
        call_member_helper(f, self,
                           std::make_index_sequence<sizeof...(args)>{},
                           argsTuple);
    }
};

template<typename R, typename ... Args>
class B: public A<R,Args...>,public std::enable_shared_from_this<B<R, Args...>>  {
public:
    B() {

    }
    template<typename T>
    void test(const T& r,Args&& ... args) {
        A<T,Args...>::test(r,std::forward<Args>(args)...);
    }

};

template<typename R,typename F,typename ...Args>
class C {

public :
    C() {
    }
    ~C() {
    }

    void test(const F& f,Args&& ...args) {
        printf("104----------\n");
        f(std::forward<Args>(args)...);
    }
};

class D {
public:
    D() {
    }
    ~D() {
    }

    void test(char* ip,int port) const {
        printf("121---------- ip = %s  port = %d \n",ip,port);
    }

    void test2(int ip,int port) {
        printf("121---------- ip = %d  port = %d \n",ip,port);
    }

};




void connectedCallBack(int clientId, NET_ERROR_CODE status,const char *msg) {
    printf("126-----clientId = %d  status = %d  msg = %s\n",clientId,status,msg);
}

void readDataCallBack(int clientId,int type, int len,uint8_t* data) {
    printf("165---收到信息 type = %d len = %d  data = %s \n",type,len,data);
}

void testNetServer() {
    std::shared_ptr<NetServer> netServer = std::make_shared<NetServer>();
    string ip = "172.20.1.298";
    netServer->setAcceptCallback(std::move(connectedCallBack));
    netServer->setReadCallback(std::move(readDataCallBack));
    netServer->listenAndAccept(ip,PORT);
}







int main() {

//    setTargetFunc(add,1,2);
//    setTargetFunc(add2,1,2,3);

testNetServer();





//    D *d = new D();
//    char* ip = "23";




//    std::function<void(char*, int)> ff2 = std::bind(&D::test, d, ip, 2);
//    ff2(ip,2);
//    decltype(&D::test);
    //std::cout<< "126-----" <<     << std::endl;


//    Task<void,char*,int>* task = new Task<void,char*,int>();
//    std::function<void(char*, int)> ff2 = std::bind(&D::test, d, ip, 2);
//    task->setTargetFunc(ff2,std::move(ip),2);
//    task->exe();
//    //task->callMemberFunction(&D::test,d,std::move(ip),1200);
//    delete task;

//    AsyncTask<int,char*,int>* asyncTask = new AsyncTask<int,char*,int>();
//    asyncTask->setTargetFunc(add,"192.168,10,200",200);
//    asyncTask->exe();
//    asyncTask->Join();
//    delete asyncTask;


//     C<int,decltype(add),std::string,int>* c = new C<int,decltype(add),std::string,int>();
//     c->test(add,"192.169",20);

//    std::tuple<char*,int> argsTuple = std::make_tuple(ip,23);
//    call_member_helper(&D::test, d,
//                       std::make_index_sequence<2>{},
//                       argsTuple);

    char* ip4 = "192.168.10.120";
//    A<void,char*,int>* a = new A<void,char*,int>();
//
//    a->callMethod(&D::test,d,std::move(ip4),123);


//     TaskHandler<void,char*,int>* th = new TaskHandler<void,char*,int>();
//     th->callMethod(&D::test,d,std::move(ip4),1200);
//     std::function<void(char*,int)> func = std::bind(&D::test,d,ip4,200);
//     th->setTargetAndArgs(func,std::move(ip4),200);
//     th->run();
//     delete th;


//    B<int,int,int>* b = new B<int,int,int>();
//    b->test(1,2,3);



//    RPCHandler rpcHandler;
//
//    // 注册回调函数
//    rpcHandler.registerCallback("add", [](const std::string& args){
//        std::istringstream is(args);
//        int a, b;
//        is >> a >> b;
//        int result = a + b;
//        std::ostringstream os;
//        os << result;
//        return os.str();
//    });
//
//    rpcHandler.registerCallback("sub", [](const std::string& args) {
//        std::istringstream is(args);
//        int a, b;
//        is >> a >> b;
//        int result = a - b;
//        std::ostringstream os;
//        os << result;
//        return os.str();
//    });
//
//    // 创建处理请求的线程
//    std::thread requestThread([&]() {
//        while (true) {
//            std::string request;
//            std::cin >> request;
//            std::string response = rpcHandler.handleRequest(request);
//            std::cout << response << std::endl;
//        }
//    });
//
//    requestThread.join();
    return 0;
}