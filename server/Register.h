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
        if (_send_buff) {
            delete _send_buff;
            _send_buff = nullptr;
        }
        printf("47----- ~Register() 被析构\n");
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
//                    if (isString(std::move(re))) {
//                        std::strstream ss;
//                        ss << re;
//                        netServer.lock()->write(proId, strlen(ss.str()),ss.str());
//                        printf("62-------string\n");
//                    } else {
//                        std::string msg = type2str(re);
//                        if (!_send_buff) {
//                            _send_buff = static_cast<char *>(malloc(msg.length()));
//                        }
//                        memset(_send_buff,0,msg.length());
//                        memcpy(_send_buff,msg.c_str(),msg.length());
//                        netServer.lock()->write(proId,msg.length(),_send_buff);
//                    }

                    std::strstream ss;
                    ss << re;
                    netServer.lock()->write(proId, strlen(ss.str()),ss.str());
                }

                printf("50--------re = %d\n",isString(re));
            } catch (const char* msg) {
                printf("转换异常，请检查解包过程  error = %s\n",msg);
            }
        };
    }

    /**
     * 注册成员函数
     * @tparam Function 成员函数 类型
     * @tparam Self     成员函数的实例对象 类型
     * @param funName   函数名称
     * @param f         成员函数
     * @param self      类实例
     */
    template <typename Function, typename Self>
    void register_member_func(std::string const& funName,  Self *self,const Function &f) {
        this->_func_map[funName] = [f,self,this](int proId,const std::weak_ptr<NetServer>& netServer,std::string str)  {
            using args_tuple_type = typename function_traits<Function>::bare_tuple_type; //获取 去除const属性 参数 类型元组
            try {
                args_tuple_type args_tuple = _unpack.get()->unpack<args_tuple_type>(str,str.length());
                auto re = call_member_func(self,f,std::move(args_tuple));
                if (!netServer.expired()) {
                    std::strstream ss;
                    ss << re;
                    netServer.lock()->write(proId, strlen(ss.str()),ss.str());
                }

                printf("110--------re = %d\n",isString(re));
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
    bool isString(std::string&& param) {
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
    char* _send_buff;
};


#endif //RPCSERVER_REGISTER_H
