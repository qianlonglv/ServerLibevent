// Server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include<winsock2.h>
#include <Windows.h>
#include <event.h>
#include <event2\event.h>
#include <event2\event_struct.h>  
#include <event2\util.h> 
#include <event2/buffer.h>  
#include <event2/bufferevent.h>  
#include <io.h>

using namespace std;

#define BUFFERSIZE 1024

typedef struct sock_event {  
	struct event *read_ev;      //读事件  
	struct event *write_ev;     //写事件  
	char *buffer;              //buffer仅存进程pid  
	int gapbeats;               //定时间隔  
	int maxbeats;               //在客户端不工作或退出的情况下，服务端的最大检测次数  
	int pastbeats;              //没有收到数据的情况下，当前的心跳检测数  
}sock_ev;  


struct event_base* g_base;        //管理所有连接事件  
int g_port = 0;
int g_ListenNum  = 0;

/* 释放堆分配的sock_ev结构体 */  
void release_sock_ev(sock_ev *ev)  
{  
	event_del(ev->read_ev);  
	free(ev->read_ev);  
	event_del(ev->write_ev);   
	free(ev->write_ev);  
	free(ev->buffer);  
	free(ev);  
}  

sock_ev* create_sock_ev(int gapbeats, int maxbeats)  
{  
	sock_ev * se = (sock_ev *)malloc(sizeof(sock_ev));    
	if (!se)  
		return NULL;  
	memset(se, 0, sizeof(sock_ev));  
	se->read_ev = (struct event*)malloc(sizeof(struct event));  
	se->write_ev = (struct event*)malloc(sizeof(struct event));  
	se->buffer =  (char *)malloc(BUFFERSIZE);  
	if (!se->read_ev || !se->write_ev || !se->buffer)  
		return NULL;  
	memset(se->read_ev, 0, sizeof(struct event));  
	memset(se->write_ev, 0, sizeof(struct event));  
	memset(se->buffer, 0, BUFFERSIZE);  
	se->gapbeats = gapbeats;  
	se->maxbeats = maxbeats;  
	se->pastbeats = 0;  

	return se;    
}  



void readcb(struct bufferevent *bev, void *ctx)
{

	struct evbuffer *input ,*output;
	char *request_line = NULL;
	char *buffer = NULL;
	size_t len = 0;

	input = bufferevent_get_input(bev);
	output = bufferevent_get_output(bev);

	size_t input_len = evbuffer_get_length(input);  
	printf("input_len: %d\n", input_len);  
	size_t output_len = evbuffer_get_length(output);  
	printf("output_len: %d\n", output_len);  

	buffer = new char[input_len];
	memset(buffer, 0, input_len);

	
	while (evbuffer_remove(input, buffer, input_len))
	{
		printf("receive data: %s\n", buffer);

// 		char *response = "receive OK!\n";
// 		evbuffer_add(output, response, strlen(response));
		
		if (strcmp(buffer, "GET") == 0)
		{
			//char *sendData = "Hello world!\n";
			evbuffer_add(output, "Hello world!\n", strlen("Hello world!\n") + 1);
		}
		if (buffer != NULL)
		{
			delete []buffer;
			buffer = NULL;
		}
		
	}


	free(request_line);
	size_t input_len1 = evbuffer_get_length(input);  
	printf("input_len1: %d\n", input_len1);  
	size_t output_len1 = evbuffer_get_length(output);  
	printf("output_len1: %d\n\n", output_len1);  

	printf("CurentThreadID : %d\n", ::GetCurrentThreadId());

}

void errorcb(struct bufferevent *bev, short error, void *ctx)  
{  
	if (error & BEV_EVENT_EOF)  
	{  
		/* connection has been closed, do any clean up here */  
		printf("connection closed\n");  
	}  
	else if (error & BEV_EVENT_ERROR)  
	{  
		/* check errno to see what error occurred */  
		printf("some other error\n");  
	}  
	else if (error & BEV_EVENT_TIMEOUT)  
	{  
		/* must be a timeout event handle, handle it */  
		printf("Timed out\n");  
	}  
	bufferevent_free(bev);  
}  


void OnAccept(evutil_socket_t  sock, short event, void *arg)
{
	struct sockaddr_in clientAddr = {};
	SOCKET ConnectSock = INVALID_SOCKET;
	int iAddrSize = 0;

	//struct timeval beat;//定时检测客户端数据 // note by guozs
	sock_ev *sockev;


	if ((sockev = create_sock_ev(1, 10)) == NULL)
	{
		return ;
	}

	iAddrSize = sizeof(struct sockaddr_in);
	ConnectSock = accept(sock, (struct sockaddr *)&clientAddr, &iAddrSize);
	if (ConnectSock == INVALID_SOCKET)
	{
        perror("Accept error");
		return;
	}

	struct bufferevent *bev = NULL;
	evutil_make_socket_nonblocking(ConnectSock);

	bev = bufferevent_socket_new(g_base, ConnectSock, BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb(bev, readcb, NULL, errorcb, NULL); 
	bufferevent_enable(bev, EV_READ|EV_WRITE);

}

int beginWork()
{
	//初始化 event base
	struct sockaddr_in server_addr = {};
	evutil_socket_t Sock = INVALID_SOCKET;
	struct event *listen_ev = NULL;

	Sock = socket(AF_INET, SOCK_STREAM, 0);
	if (Sock == INVALID_SOCKET)
	{
		perror("create socket failed");
		system("PAUSE ");
		return 0;
	}
	else
	{
		int iFlag = 1, iLen = sizeof(int);
		if(setsockopt(Sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&iFlag , iLen) != 0)
		{
			int iErrorno = GetLastError();
			cout<<"设置socket 属性失败---------错误码为"<<iErrorno<<'\n';
			system("PAUSE ");
			return 0;
		}
		else
		{
			memset(&server_addr, 0, sizeof(server_addr));
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(g_port);
			server_addr.sin_addr.s_addr = INADDR_ANY;

			if(bind(Sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
			{
				perror("bind socket error");
				system("PAUSE");
				return 0;
			}

			if (listen(Sock, 10) == -1)
			{
				perror("listen socket error");
				system("PAUSE");
				return 0;
			}

			g_base = event_base_new();
			listen_ev = event_new(g_base, Sock, EV_READ|EV_PERSIST, OnAccept, NULL);
			if (!listen_ev)
			{
				printf("event_new fail\n");
				system("PAUSE");
				return 0;
			}

			event_add(listen_ev, NULL);
			event_base_dispatch(g_base);

			if(event_del(listen_ev) == 0)
			{ 
				event_free(listen_ev);      //删除事件,释放空间  
			}  

			event_base_free(g_base);          //删除base对象  
		}

	}
}

void LoadConfig()
{

	char *sConfigName = _T("Config.ini");
	if (access(sConfigName, 0) != 0)
	{
		printf("配置文件不存在!\n");
		return;
	}

	char sBuffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, sBuffer);

	char sConfigPath[100];
	sprintf_s(sConfigPath, 100, "%s\\%s", sBuffer, sConfigName);
	g_ListenNum = ::GetPrivateProfileInt("Config", "ListenNum", 10, sConfigPath);
	g_port = ::GetPrivateProfileInt("Config", "ListenPort", 5222, sConfigPath);
}

int _tmain(int argc, _TCHAR* argv[])
{

	WORD wVersionRequested;
	WSADATA wsaData;
	//WinSock初始化
	wVersionRequested=MAKEWORD(2, 2); //希望使用的WinSock DLL 的版本
	int ret=WSAStartup(wVersionRequested, &wsaData);
	if(ret!=0)
	{
		printf("WSAStartup() failed!\n");
		return 0;
	}


	LoadConfig();


	beginWork();
	

	return 0;
}

