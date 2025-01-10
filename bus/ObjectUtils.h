//
// Created by Rambo.Liu on 2024/12/30.
//

#ifndef RPCSERVER_OBJECTUTILS_H
#define RPCSERVER_OBJECTUTILS_H

/** 不支持当前功能 */
#define CGRAPH_NO_SUPPORT                                               \
    return CErrStatus(CGRAPH_FUNCTION_NO_SUPPORT);                      \

/** 定义为不能赋值和拷贝的对象类型 */
#define CGRAPH_NO_ALLOWED_COPY(CType)                                   \
    CType(const CType &) = delete;                                      \
    const CType &operator=(const CType &) = delete;                     \

/** 抛出异常 */
#define CGRAPH_THROW_EXCEPTION(info)                                    \
    throw CException(info, CGRAPH_GET_LOCATE);                          \




#endif //RPCSERVER_OBJECTUTILS_H
