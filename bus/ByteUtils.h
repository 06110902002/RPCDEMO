//
// Created by Rambo.Liu on 2025/1/3.
//

#ifndef RPCSERVER_BYTEUTILS_H
#define RPCSERVER_BYTEUTILS_H


#include <cstdint>

class ByteUtils {

public:

    static uint8_t *int2Bytes(int i);

    static long byte2long(uint8_t *bytes);

    static int byte2Int(uint8_t *bytes);

    /**
     * 读取指定socket通道的指定长度数据
     *
     * @param length
     * @param clientfd
     * @return 返回读取的长度
     */
    static uint8_t *readLengthForClient(int length, int clientfd);
};


#endif //RPCSERVER_BYTEUTILS_H
