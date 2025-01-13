//
// Created by Rambo.Liu on 2024/12/19.
// 函数萃取帮助类

#ifndef RPCSERVER_META_H
#define RPCSERVER_META_H

#include <iostream>
#include <tuple>
#include <type_traits>


//定义基本模板
template<typename T>
struct function_traits;

//// 特化用于普通函数类型
//template<typename R, typename... Args>
//struct function_traits<R(Args...)> {
//    using result_type = R;
//    static const std::size_t arity = sizeof...(Args);
//
//    template<std::size_t N>
//    struct arg {
//        static_assert(N < arity, "参数索引超出范围");
//        using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
//    };
//
//    using tuple_type = std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...>;
//    using bare_tuple_type = std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...>;
//};
// 普通函数
template<typename ReturnType, typename... Args>
struct function_traits<ReturnType(Args...)>
{
    enum { arity = sizeof...(Args) };
    using return_type = ReturnType;
    using function_type = ReturnType(Args...);
    using stl_function_type = std::function<function_type>;
    using pointer = ReturnType(*)(Args...);

    template<size_t I>
    struct args
    {
        static_assert(I < arity, "index is out of range, index must less than sizeof Args");
        using type = typename std::tuple_element<I, std::tuple<Args...>>::type;
    };

    using tuple_type = std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...>;
    using bare_tuple_type = std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...>;
};

//普通函数
//template <typename Ret> struct function_traits<Ret()> {
//public:
//    enum { arity = 0 };
//    typedef Ret function_type();
//    typedef Ret return_type;
//    using stl_function_type = std::function<function_type>;
//    typedef Ret (*pointer)();
//
//    typedef std::tuple<> tuple_type;
//    typedef std::tuple<> bare_tuple_type;
//    using args_tuple = std::tuple<std::string>;
//    using args_tuple_2nd = std::tuple<std::string>;
//};

// 特化用于普通函数指针
template<typename R, typename... Args>
struct function_traits<R(*)(Args...)> : function_traits<R(Args...)> {
};

// 特化用于成员函数指针
template<typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)> : function_traits<R(Args...)> {
};

// 特化用于 const 成员函数指针
template<typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const> : function_traits<R(Args...)> {
};


// 特化用于 Lambda 表达式和函数对象
template<typename T>
struct function_traits : function_traits<decltype(&T::operator())> {
};


/****-----------------std:make_index_sequence  遍历std:tuple--------------------*/
template <typename Tuple, typename Func, size_t ... N>
void func_call_tuple(const Tuple& t, Func&& func, std::index_sequence<N...>) {
    static_cast<void>(std::initializer_list<int>{(func(std::get<N>(t)), 0)...});
}

template <typename ... Args, typename Func>
void travel_tuple(const std::tuple<Args...>& t, Func&& func) {
    func_call_tuple(t, std::forward<Func>(func), std::make_index_sequence<sizeof...(Args)>{});
}
/****-----------------std:make_index_sequence  遍历std:tuple--------------------*/

/**--------------------------遍历tuple 并将tuple 当作函数参数列表执行-----------------------------***/
template<typename Function, typename Tuple, std::size_t... Index>
decltype(auto) invoke_impl(Function &&func, Tuple &&t, std::index_sequence<Index...>) {
    return func(std::get<Index>(std::forward<Tuple>(t))...);
}

template<typename Function, typename Tuple>
decltype(auto) callMethod(Function &&func, Tuple &&t) {
    constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
    return invoke_impl(std::forward<Function>(func), std::forward<Tuple>(t), std::make_index_sequence<size>{});
}
/**--------------------------遍历tuple 并将tuple 当作函数参数列表执行-----------------------------***/



/**
 * 调用成员函数
 * @tparam F       成员函数
 * @tparam Self    实例对象
 * @tparam Indexes 参数的索引
 * @tparam Args    参数列表
 * @param f        成员函数地址
 * @param self     实例对象
 * @param tup      成员函数参数元组
 */
template<typename F, typename Self, size_t... Indexes, typename... Args>
static auto call_member_helper(const F &f, Self *self,
                               std::index_sequence<Indexes...>,
                               std::tuple<Args...> tup) {
    //std::cout << "Indexes 0 = " << std::get<0>(tup) << " 1= " << std::get<1>(tup) << std::endl;
    std::cout << "call_member_helper  传进来的参数" << std::endl;
    travel_tuple(tup, [](auto&& item) {
        std::cout << item << " ,";
    });
    std::cout << std::endl;

    return (*self.*f)(std::move(std::get<Indexes>(tup))...);
}

template<typename Self, typename Function,typename Tuple, std::size_t... Index>
decltype(auto) invoke_member_impl(Self *self,Function &&func,Tuple &&t, std::index_sequence<Index...>) {
    return (*self.*func)(std::move(std::get<Index>(t))...);
}

template<typename F, typename Self, typename Tuple>
static auto call_member_func(Self *self, F &&f, Tuple&& tup) {
//    std::cout << "call_member_func  传进来的参数" << std::endl;
//    travel_tuple(tup, [](auto&& item) {
//        std::cout << item << " ,";
//    });
//    std::cout << std::endl;
    constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
    return invoke_member_impl(self,std::forward<F>(f), std::forward<Tuple>(tup), std::make_index_sequence<size>{});
}
/**----------------------通用函数调用器--------------------------*/

#endif //RPCSERVER_META_H
