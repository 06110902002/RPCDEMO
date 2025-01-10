cmake_minimum_required(VERSION 3.23)
project(RPCServer)

set(CMAKE_CXX_STANDARD 17)
include_directories(
        "./net/*.h"
        "./server/*.h"
        "meta.h"
        bus/jsonxx.h
)

#客户端入口
add_executable(RPCClient
        client/main.cpp
        client/Request.h
        client/RPCClient.h
        client/StringUtils.h
        client/SocketClient.h
        client/Object.h
        client/AtomicQueue.h
        bus/ObjectUtils.h
        client/StdExd.h
        client/TaskPack.h
        client/TaskMgr.cpp
        client/TaskMgr.h
        bus/ByteUtils.cpp
        bus/ByteUtils.h
        bus/UnPack.h
        bus/jsonxx.cpp
        )

#服务端
add_executable(
        RPCServer
        #server/BaseTask.h
        server/main.cpp

#        server/Task.h
#        meta.h
#        server/TaskHandler.h
#        server/AsyncTask.h
        server/Thread.cpp
#        server/Thread.h


        net/Socket.cpp
        net/OsHandle.cpp
        net/Address.cpp
        net/NetServer.cpp
        net/Dispatcher.cpp
#        net/Socket.h
#        net/NetType.h
#        net/OsHandle.h
#        net/Address.h
#        net/NetServer.h
#        net/Dispatcher.h
#        net/Response.h
        server/Register.h
        bus/UnPack.h
        bus/jsonxx.cpp
)
