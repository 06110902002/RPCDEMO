//
// Created by Rambo.Liu on 2024/12/24.
// 封装客户端的网络请求
// 将string 转换为 json 格式 ，如方法：add (int a,int b) 转换为json：
// {
//    "method": "add",
//    "args": [
//        1,
//        2
//    ]
//}

#ifndef RPCSERVER_REQUEST_H
#define RPCSERVER_REQUEST_H

#include <iostream>
#include <sstream>

template<typename ...Args>
class Request : public std::enable_shared_from_this<Request<Args...>> {
public:

    Request() {

    }

    ~Request() {
        printf("~Request 被析构\n");
    }

    std::string convert2JSONStr(std::string method,Args &&... args) {
        const std::size_t n = sizeof...(Args);
        if (n > 0) {
            std::ostringstream oss;
            std::string sep;
            oss << "{";
            oss << sep << "\"method\"";
            oss << ":";
            oss << "\"";
            oss << sep << method;
            oss << "\"";
            oss << ",";
            oss << "\"";
            oss << "args";
            oss << "\"";
            oss << ":";
            oss << "[";
            ([&](const auto &value) {
                if(isString(value)) {
                    sep = ",\"";
                    oss << sep << value;
                    oss << "\"";
                } else {
                    oss << sep << value;
                }
                sep = ", ";
            }(std::forward<Args>(args)), ...); //逗号表达式，将参数展开
            oss << "]";
            oss << "}";
            return oss.str();
        }
        return "";

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



};


#endif //RPCSERVER_REQUEST_H
