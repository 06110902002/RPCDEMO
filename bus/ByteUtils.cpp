//
// Created by Rambo.Liu on 2025/1/3.
//

#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include "ByteUtils.h"

uint8_t *ByteUtils::int2Bytes(int i) {
    uint8_t *buf = (uint8_t *) malloc(4 * sizeof(uint8_t));
    memset(buf, 0, 4);
    buf[0] = (unsigned char) i;
    buf[1] = i >> 8;
    buf[2] = i >> 16;
    buf[3] = i >> 24;
    return buf;
}

long ByteUtils::byte2long(uint8_t *bytes) {
    long value = ((bytes[0] & 0xFF)
                  | ((bytes[1] & 0xFF) << 8)
                  | ((bytes[2] & 0xFF) << 16)
                  | ((bytes[3] & 0xFF) << 24));
    return value;
}

int ByteUtils::byte2Int(uint8_t *bytes) {
    int value;
    value = (int) ((bytes[0] & 0xFF)
                   | ((bytes[1] & 0xFF) << 8)
                   | ((bytes[2] & 0xFF) << 16)
                   | ((bytes[3] & 0xFF) << 24));
    return value;
}

uint8_t *ByteUtils::readLengthForClient(int length, int clientfd) {
    if (length <= 0 || clientfd < 0) {
        return nullptr;
    }
    uint8_t *buf = (uint8_t *) malloc(length * sizeof(uint8_t));
    memset(buf, 0, length);
    int read = 0;
    while (read < length) {
        int cur = 0;
        //LOGV( "总长度 = %d  已经读取  = %d",length, read);
        cur = recv(clientfd, buf, length - read, 0);
        if (cur <= 0) {
            free(buf);
            buf = nullptr;
            break;
        }
        read += cur;
    }
    return buf;
}
