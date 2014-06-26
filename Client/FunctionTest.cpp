#include "stdafx.h"

/***
    TCP基本连通测试
    hostip: 服务端IP
    port: 服务端端口

    return: 0,如果测试成功
*/
int tcpBasicTest(const char *hostip, u_short port)
{
    const int BUF_SIZE = 64;    // 接收缓冲区大小

    int recvNum = 0;
    char *buf = "hello, world";
    char bufRecv[BUF_SIZE];
    SOCKET sHost;

    // 建立连接
    sHost = connectServer(hostip, port);
    if (sHost == NULL)
    {
        return -1;
    }

    // 发送数据
    if (SendtoServer(sHost, buf, strlen(buf) + 1))
    {
        return -1;
    }

    // 接收数据
    recvNum = RecvFromServer(sHost, bufRecv, BUF_SIZE);
    if (recv < 0)
    {
        return -1;
    }
    if (recvNum == BUF_SIZE)
    {
        recvNum = BUF_SIZE - 1;
    }
    bufRecv[recvNum] = '\0';
    printf("RECV: %s\n", bufRecv);

    // 关闭连接
    connectionClose(sHost);
    return 0;
}

/***
    单连接循环发送
*/
int SingleLinkTest(const char *hostip, u_short port)
{
    return 0;
}