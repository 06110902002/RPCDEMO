//
// Created by Rambo.Liu on 2024/12/26.
// 将string 转换为想要的类型


#ifndef RPCSERVER_STRINGUTILS_H
#define RPCSERVER_STRINGUTILS_H

#include <iostream>

template<typename In, typename Out>
class StringUtils : public std::enable_shared_from_this<StringUtils<In, Out>> {
public:
    StringUtils() {

    }

    ~StringUtils() {
        printf("~StringUtils 被析构\n");
    }

    Out convert(const In &value) {
        return test(std::forward<const In &>(value));
    }


    Out test(const In &value) {
        if constexpr (std::is_integral<Out>::value) {
            return testInt(value);
        } else if constexpr (std::is_floating_point<Out>::value) {
            return testFloat(value);
        } else {
            return testStr((std::string &) value);
        }
    }

    template<typename T, typename = typename std::enable_if<!std::is_integral<T>::value>::type>
    Out testStr(T &str) {
        printf("120-------这是字符串api str = %s\n", str.c_str());
        return str;
    }

    //使用std::enable_if 校验模板参数 类型
    int testInt(const In &info) {
        printf("124-------这是整型api info = %d\n", std::stoi(info));
        return std::stoi(info);
    }

    float testFloat(const In &info) {
        printf("189-------这是floatapi info = %f\n", std::stof(info));
        return std::stof(info);
    }

    double testDouble(const In &info) {
        printf("189-------Double info = %f\n", std::stod(info));
        return std::stod(info);
    }
};



#endif //RPCSERVER_STRINGUTILS_H
