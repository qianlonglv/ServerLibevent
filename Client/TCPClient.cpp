#include "stdafx.h"

#pragma comment(lib, "ws2_32.lib") 

/***
    建立Socket连接
    hostip: 服务端IP
    port:   服务端端口

    return: Socket
*/
SOCKET connectServer(const char *hostip, u_short port)
{
    WSADATA         wsd;            //WSADATA变量  
    SOCKET          sHost;          //服务器套接字  
    SOCKADDR_IN     servAddr;       //服务器地址  

    //初始化套结字动态库  
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)  
    {  
        perror("WSAStartup failed");
        return NULL;  
    }  
      
    //创建套接字  
    sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    
    if(INVALID_SOCKET == sHost)  
    {  
        perror("socket failed");
        WSACleanup();//释放套接字资源  
        return  NULL;  
    }  

    //设置服务器地址  
    servAddr.sin_family =AF_INET;  
    servAddr.sin_addr.s_addr = inet_addr(hostip);  
    servAddr.sin_port = htons(port);  
    int nServAddlen  = sizeof(servAddr);  

    //连接服务器  
    if (connect(sHost,(LPSOCKADDR)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        perror("connect failed");
        closesocket(sHost); //关闭套接字  
        WSACleanup();       //释放套接字资源  
        return NULL;  
    }

    return sHost;
}

/***
    关闭Socket连接
    s: 要关闭的socket
*/
void connectionClose(SOCKET s)
{
    closesocket(s); // 关闭套接字
    WSACleanup();       //释放套接字资源  
}

/***
    发送数据
    s:      socket
    buf:    待发送的数据
    buflen: 数据长度

    return: 0，如果发送成功
*/
int SendtoServer(SOCKET s, const char *buf, int buflen)
{
    if (send(s, buf, buflen, 0) == SOCKET_ERROR)
    {
        perror("send failed");
        connectionClose(s);
        return -1;
    }

    return 0;
}

/***
    接收数据
    s:      socket
    buf:    接收数据缓冲区
    buflen: 缓冲区长度

    return: 接收到的数据长度，小于0为接收错误
*/
int RecvFromServer(SOCKET s, char *buf, int buflen)
{
    int n = 0;

    n = recv(s, buf, buflen, 0);
    if (n < 0)
    {
        perror("recv failed");
        connectionClose(s);
    }

    return n;
}