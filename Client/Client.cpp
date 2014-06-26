// Client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
    const char *SERVERADDR = "127.0.0.1";
    const u_short PORT = 5222;

    // 基本连接测试
    if (tcpBasicTest(SERVERADDR, PORT))
    {
        printf("Failed to test tcp connetion\n");
    }

    getchar();
	return 0;
}
