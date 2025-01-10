//
// Created by Rambo.Liu on 2025/1/8.
// 解包字符器，得到一个std::tuple
// {
//    "method": "add",
//    "args": [
//        1,
//        2
//    ]
//}

#ifndef RPCSERVER_UNPACK_H
#define RPCSERVER_UNPACK_H

#include <iostream>
#include <vector>
#include "jsonxx.h"
#include <sstream>


/**
 * 横版特化处理
 * 使用递归将jsonxx::Array 转换为std:tuple
 * @tparam Tuple
 * @tparam N
 */
template <typename Tuple, std::size_t N>
struct StdTupleConverter {
    static void convert(jsonxx::Array& o,Tuple& v) {
        StdTupleConverter<Tuple, N-1>::convert(o, v);
        if (o.size() >= N) {
            using e_type = decltype(std::get<N-1>(v));
            e_type e_value = o.get<typename std::remove_reference<decltype(std::get<N-1>(v))>::type>(N-1);
            std::get<N-1>(v) = e_value;
        }
    }
};

template <typename Tuple>
struct StdTupleConverter<Tuple, 0> {
    static void convert (jsonxx::Array const&,Tuple&) {
        printf("递归停止\n");
    }
};



class UnPack : public std::enable_shared_from_this<UnPack> {

public:
    UnPack() {

    }

    ~UnPack() {

    }

    // 将JSON数组转换为tuple的函数
    template<typename Tuple, std::size_t... I>
    Tuple json2tupleImpl(const jsonxx::Array& j, Tuple& tuple,std::index_sequence<I...>) {
        return std::make_tuple(j.get<typename std::remove_reference<decltype(std::get<I>(tuple))>::type>(I)...);
    }

    /**
     * 将数组转换为 std::tuple
     * @tparam Tuple 目标std::tuple
     * @param j      数组
     * @param tuple  存放转换后的结果
     * @return
     */
    template<typename Tuple>
    Tuple json2tuple(const jsonxx::Array& j,Tuple& tuple) {
        return json2tupleImpl<Tuple>(j, tuple,std::make_index_sequence<std::tuple_size<Tuple>::value>{});
    }



    /**
     * 解包字符串  得到std::tuple
     * @tparam T     实质为std::tuple<类型1，...类型n>
     * @param data   待解包的json
     * @param length 长度
     * @return       解包好的 std::tuple<类型1，...类型n>
     * 注意的是：jsonxx::array 中类型需要  被调用函数的形参命名需要一致
     * 比如：jsonxx::array 中Number 代表的long double  ,如果函数使用了 int 来代表数字则会异常
     */
    template<typename T>
    T unpack(std::string& data, size_t length) {
        try {
            jsonxx::Object o;
            o.parse(data);
            jsonxx::Array args_array = o.get<jsonxx::Array>("args");
            T arg_tuple = json2tuple<T>(args_array,arg_tuple);
            return arg_tuple;
        } catch (...) {
            throw std::invalid_argument("unpack failed: Args not match!");
        }
    }



};

#endif //RPCSERVER_UNPACK_H
