//
// Created by Rambo.Liu on 2024/12/25.
//

#ifndef RPCSERVER_RPCCLIENT_H
#define RPCSERVER_RPCCLIENT_H

#include <iostream>
#include <future>
#include "Request.h"

const constexpr size_t DEFAULT_TIMEOUT = 5000; // milliseconds

enum StatusCode {
    NET_DISCONNECTED = 1,    // remote close the socket.
    METHOD_NOT_EXITS = 2,       // 方法不存在
    SUCCESS = 3,
};


template<typename T>
struct future_result {
    std::future<T> future;

    template<class Rep, class Per>
    std::future_status wait_for(const std::chrono::duration<Rep, Per> &rel_time) {
        return future.wait_for(rel_time);
    }

    T get() {
        return future.get();
    }
};

enum class CallModel {
    future,
    callback
};
const constexpr auto FUTURE = CallModel::future;


class RPCClient : public std::enable_shared_from_this<RPCClient> {
public:
    RPCClient() {

    }

    ~RPCClient() {
        printf("RPCClient 被析构\n");
    }

    /**
     * sync call  返回值为void
     * @tparam TIMEOUT
     * @tparam T
     * @tparam Args
     * @param rpc_name
     * @param args
     * @return
     */
    template<size_t TIMEOUT, typename T = void, typename... Args>
    typename std::enable_if<std::is_void<T>::value>::type
    call(const std::string &rpc_name, Args &&...args) {
        auto future_result = async_call<FUTURE>(rpc_name, std::forward<Args>(args)...);
        auto status = future_result.wait_for(std::chrono::milliseconds(TIMEOUT));
        if (status == std::future_status::timeout ||
            status == std::future_status::deferred) {
            throw std::out_of_range("timeout or deferred");
        }

        future_result.get().as();
    }

    /**
    * sync call  返回值为void
    * @tparam TIMEOUT  DEFAULT_TIMEOUT
    * @tparam T
    * @tparam Args
    * @param rpc_name
    * @param args
    * @return
    */
    template<typename T = void, typename... Args>
    typename std::enable_if<std::is_void<T>::value>::type
    call(const std::string &rpc_name, Args &&...args) {
        call<DEFAULT_TIMEOUT, T>(rpc_name, std::forward<Args>(args)...);
    }

    /**
     * async call 返回值不空
     * @tparam TIMEOUT
     * @tparam T
     * @tparam Args
     * @param rpc_name
     * @param args
     * @return
     */
    template<size_t TIMEOUT, typename T, typename... Args>
    typename std::enable_if<!std::is_void<T>::value, T>::type
    call(const std::string &rpc_name, Args &&...args) {
        auto future_result =
                sync_call<FUTURE>(rpc_name, std::forward<Args>(args)...);
        auto status = future_result.wait_for(std::chrono::milliseconds(TIMEOUT));
        if (status == std::future_status::timeout ||
            status == std::future_status::deferred) {
            printf("104-----------timeout\n");
            throw std::out_of_range("timeout or deferred");
        }

        return future_result.get();
    }

    template<typename T, typename... Args>
    typename std::enable_if<!std::is_void<T>::value, T>::type
    call(const std::string &rpc_name, Args &&...args) {
        return call<DEFAULT_TIMEOUT, T>(rpc_name, std::forward<Args>(args)...);
    }

    /**
     * 异步调用
     * @tparam T   结果返回类型
     * @tparam Args
     * @param rpc_name
     * @param args
     * @return
     */
    template<CallModel model, typename... Args>
    future_result<std::string> sync_call(const std::string &rpc_name, Args &&...args) {
        uint64_t fu_id = 0;
        std::packaged_task<std::string()> task([&]() {
            Request<Args...> request;
            std::string result = request.convert2JSONStr(rpc_name,std::forward<Args>(args)...);
            std::this_thread::sleep_for(std::chrono::seconds(3));
            return result;
        });
        std::future<std::string> f1 = task.get_future();
        std::thread(std::move(task)).detach();
        return future_result<std::string>{std::move(f1)};
    }


    template<typename... Args>
    void async_call(const std::string &rpc_name,
                    std::function<void(StatusCode, std::string)> cb,
                    Args &&...args) {
        std::packaged_task<void()> task([cb]() {
            std::cout << "\n 异常调用 packaged tid = " << std::this_thread::get_id() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
            if (cb) {
                cb(SUCCESS, "this is test msg");
            }
        });
        std::thread task_td(std::move(task));
        task_td.join();

    }

};

#endif //RPCSERVER_RPCCLIENT_H
