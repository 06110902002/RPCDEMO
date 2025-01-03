//
// Created by Rambo.Liu on 2024/12/24.
//

#ifndef RPCSERVER_RESPONSE_H
#define RPCSERVER_RESPONSE_H
#include "iostream"

template<typename Cls, typename ...Args>
class Response {

public:

    Response() {

    }
    ~Response() {
        printf("~Response 被析构\n");
    }

};

#endif //RPCSERVER_RESPONSE_H
