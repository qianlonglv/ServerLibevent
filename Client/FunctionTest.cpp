#include "stdafx.h"

/***
    TCP������ͨ����
    hostip: �����IP
    port: ����˶˿�

    return: 0,������Գɹ�
*/
int tcpBasicTest(const char *hostip, u_short port)
{
    const int BUF_SIZE = 64;    // ���ջ�������С

    int recvNum = 0;
    char *buf = "hello, world";
    char bufRecv[BUF_SIZE];
    SOCKET sHost;

    // ��������
    sHost = connectServer(hostip, port);
    if (sHost == NULL)
    {
        return -1;
    }

    // ��������
    if (SendtoServer(sHost, buf, strlen(buf) + 1))
    {
        return -1;
    }

    // ��������
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

    // �ر�����
    connectionClose(sHost);
    return 0;
}

/***
    ������ѭ������
*/
int SingleLinkTest(const char *hostip, u_short port)
{
    return 0;
}