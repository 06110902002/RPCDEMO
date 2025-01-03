//
// Created by Rambo.Liu on 2024/12/30.
//

#ifndef RPCSERVER_STDEXD_H
#define RPCSERVER_STDEXD_H

#include <type_traits>
#include <iostream>

// 兼容 std::make_unique 的语法
template<typename T, typename... Args>
typename std::unique_ptr<T> c_make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif //RPCSERVER_STDEXD_H
