// Client.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
    const char *SERVERADDR = "127.0.0.1";
    const u_short PORT = 5222;

    // �������Ӳ���
    if (tcpBasicTest(SERVERADDR, PORT))
    {
        printf("Failed to test tcp connetion\n");
    }

    getchar();
	return 0;
}
