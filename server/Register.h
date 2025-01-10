//
// Created by Rambo.Liu on 2024/12/24.
// 注册函数名称与函数方法

#ifndef RPCSERVER_REGISTER_H
#define RPCSERVER_REGISTER_H
#include <iostream>
#include <unordered_map>
#include <utility>
#include <strstream>
#include "../bus/UnPack.h"

class Register: public std::enable_shared_from_this<Register> {


    struct taskBased {
        explicit taskBased() = default;

        virtual void call() = 0;

        virtual ~taskBased() = default;
    };

    template<typename F, typename T = typename std::decay<F>::type>
    struct taskDerided : taskBased {
        T func_;

        explicit taskDerided(F &&func) : func_(std::forward<F>(func)) {}

        void call() override {
            func_();
        }
    };


public:

    Register() {
        _unpack = std::make_shared<UnPack>();
    }

    ~Register() {

    }

    template<typename Function>
    void register_nonmember_func(std::string const& funName,Function f) {
        this->_func_map[funName] = [f,this](int proId,const std::weak_ptr<NetServer>& netServer,std::string str) {
            //萃取 F 函数的  形参类型元组，如果函数 add (int a,int b) ,则bare_tuple_type = std::tuple<int,int>
            using args_tuple_type = typename function_traits<Function>::bare_tuple_type;
            try {
                args_tuple_type args_tuple = _unpack.get()->unpack<args_tuple_type>(str,str.length());
                auto re = callMethod(f,std::move(args_tuple));
                //执行完毕之后 需要将结果沿对应的socket 返回
                if (!netServer.expired()) {
                    if(isString(re)) {
                        //netServer.lock()->write(proId,re.length(),re.c_str());
                    } else {
                        std::string msg = type2str(re);
                        printf("60--------msg = %s\n",msg.c_str());
                        netServer.lock()->write(proId,msg.length(),msg.c_str());
                    }
                }

                printf("50--------re = %d\n",isString(re));
            } catch (const char* msg) {
                printf("转换异常，请检查解包过程  error = %s\n",msg);
            }
        };
    }

    /**
     * 根据客户端发来的函数名，调用，并将结果返回给客户端
     * @param funcName  函数名
     * @param proId     当前协议id
     * @param netServer socket
     * @param args      客户端传递来的参数列表，需要将参数列表解包成一个std:tuple
     */
    void exe(std::string& funcName,int proId,std::weak_ptr<NetServer> netServer,std::string& args) {
        auto it = _func_map.find(funcName);
        if (it == _func_map.end()) {
            printf("56------method unregister\n");
        } else {
            it->second(proId,std::move(netServer),args);
        }
    }



    template<typename T>
    bool isString(T&&) {
        return false;
    }

    /**
     * 函数特化处理是否为字符串
     * @param param
     * @return
     */
    template<>
    bool isString(const std::string& param) {
        return true;
    }

    template<class T>
    std::string type2str(T src) {
        std::strstream ss;
        ss << src;
        std::string ret;
        ss >> ret;
        return ret;
    }



//    template<typename FunctionType>
//    auto commit(const FunctionType &func)
//    -> std::future<decltype(std::declval<FunctionType>()())> {
//        using ResultType = decltype(std::declval<FunctionType>()());
//        std::packaged_task<ResultType()> task(func);
//        std::future<ResultType> result(task.get_future());
//        pushTask(std::move(task));
//        return result;
//    }



protected:
    //禁止拷贝与赋值 构造对象
    Register(const Register &) = delete;
    Register &operator=(const Register &) = delete;


    std::unique_ptr<taskBased> impl_ = nullptr;
    std::unordered_map<std::string, std::function<void(int proId,std::weak_ptr<NetServer> netServer,std::string)>> _func_map;

private:
    std::shared_ptr<UnPack> _unpack;
};


#endif //RPCSERVER_REGISTER_H
