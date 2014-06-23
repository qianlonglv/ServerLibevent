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

using namespace std;

#define PORT 5222
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

void socket_write(evutil_socket_t  sock, short event, void* arg)  
{  
	char* buffer;  
	if (!arg)  
		return;  

	buffer = (char *)arg;  
	if (send(sock, buffer, sizeof(*buffer), 0) < 0) {  

		printf("server send msg error: errno %d--%s\n", errno, strerror(errno));      
		return;   
	}  
	memset(buffer, 0, sizeof(*buffer));  
}  


void socket_read(evutil_socket_t  sock, short event, void* arg)  
{  
	int size = 0;  
	sock_ev* sockev = (sock_ev*)arg;  
	if(!sockev)  
		return;  

	memset(sockev->buffer, 0, BUFFERSIZE);  

	while (1)
	{
		size = recv(sock, sockev->buffer, BUFFERSIZE, 0);
		if (size <= 0)
		{
			break;
		}

		printf("receive data:%s   size:%d\n", sockev->buffer, size);
	}

	if (size <= 0)
	{
		sockev->pastbeats++;
		if (sockev->pastbeats >= sockev->maxbeats)
		{
			release_sock_ev(sockev);
			closesocket(sock);
		}

		return ;
	}

	// 	sockev->pastbeats = 0;  
	// 	printf("pastbeats:\t%d\n", sockev->pastbeats);           //--debug  
	// 	printf("receive data:\t%s   size:\t%d\n", sockev->buffer, size);  
	// 	event_add(sockev->write_ev, NULL);           //添加端口写事件，将数据返回给客户端  
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
	/*while (request_line = evbuffer_readln(input, &len, EVBUFFER_EOL_CRLF))*/
	while (evbuffer_remove(input, buffer, input_len))
	{
		printf("receive data: %s\n", buffer);
		delete []buffer;
		buffer = NULL;

		char *response = "receive OK!\n";
		evbuffer_add(output, response, strlen(response));
	}


	free(request_line);
	size_t input_len1 = evbuffer_get_length(input);  
	printf("input_len1: %d\n", input_len1);  
	size_t output_len1 = evbuffer_get_length(output);  
	printf("output_len1: %d\n\n", output_len1);  

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

	struct timeval beat;//定时检测客户端数据
	sock_ev *sockev;


	if ((sockev = create_sock_ev(1, 10)) == NULL)
	{
		return ;
	}

	iAddrSize = sizeof(struct sockaddr_in);
	ConnectSock = accept(sock, (struct sockaddr *)&clientAddr, &iAddrSize);
	if (ConnectSock == INVALID_SOCKET)
	{
		printf("Accept error: errno %d --- %s\n", errno, strerror(errno));
		return;
	}

	struct bufferevent *bev = NULL;
	evutil_make_socket_nonblocking(sock);

	bev = bufferevent_socket_new(g_base, ConnectSock, BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb(bev, readcb, NULL, errorcb, NULL); 
	bufferevent_enable(bev, EV_READ|EV_WRITE);
	// 	event_assign(sockev->read_ev, g_base, ConnectSock, EV_PERSIST, socket_read, sockev);
	// 	evutil_timerclear(&beat);  
	// 	beat.tv_sec = sockev->gapbeats;           //定期检查端口是否可读,来判断客户端是否存在  
	// 	event_add(sockev->read_ev, &beat);  
	// 
	// 	event_assign(sockev->write_ev, g_base, ConnectSock, EV_WRITE, socket_write, sockev->buffer);      

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

	//初始化 event base
	struct sockaddr_in server_addr = {};
	evutil_socket_t Sock = INVALID_SOCKET;
	struct event *listen_ev = NULL;

	Sock = socket(AF_INET, SOCK_STREAM, 0);
	if (Sock == INVALID_SOCKET)
	{
		printf("create socket failed! errno : %d --- %s\n", errno, strerror(errno));
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
			server_addr.sin_port = htons(PORT);
			server_addr.sin_addr.s_addr = INADDR_ANY;

			if(bind(Sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
			{
				printf("bind socket error:errno %d -- %s \n", errno, strerror(errno));
				system("PAUSE");
				return 0;
			}

			if (listen(Sock, 10) == -1)
			{
				printf("listen socket error : errno %d -- %s\n", errno, strerror(errno));
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

	return 0;
}

