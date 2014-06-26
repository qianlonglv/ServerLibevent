// Client.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ServerApi.h"
#include <vector>
#include <process.h>
using namespace std;

#pragma comment(lib,"ServerApi.lib")

UINT WINAPI ThreadGetData(LPVOID lp)
{
	UINT dwThreadID = GetCurrentThreadId();
	SOCKET *pSock = (SOCKET *)lp;

	string sContent;
	getString(*pSock , sContent);
	printf("�յ�����˷��������ݣ�%s ThreadID : %d\n", sContent, dwThreadID);

	return 0;

}

int _tmain(int argc, _TCHAR* argv[])
{

	WORD wVersionRequested;
	WSADATA wsaData;
	//WinSock��ʼ��
	wVersionRequested=MAKEWORD(2, 2); //ϣ��ʹ�õ�WinSock DLL �İ汾
	int ret=WSAStartup(wVersionRequested, &wsaData);
	if(ret!=0)
	{
		printf("WSAStartup() failed!\n");
		return 0;
	}


    const char *sIP = "127.0.0.1";
    const int iPort = 5222;
	vector<SOCKET *> vSocket;	

	for (int i = 0; i < 200; i++)
	{	
		SOCKET *pSock = new SOCKET;
		*pSock = INVALID_SOCKET;
		int iResult = ConnectServer(*pSock, sIP, iPort);
		if (iResult == TRUE)
		{
			vSocket.push_back(pSock);

			UINT iThreadID = 0;
			HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, &ThreadGetData, pSock, 0, &iThreadID);

		}

	}

	for (int i = 0; i < 200; i++)
	{
		SOCKET *sockTmp = vSocket[i];
		if (sockTmp != NULL)
		{
			//disConnectServer(sockTmp);
			
			*sockTmp = INVALID_SOCKET;
			delete sockTmp;
			sockTmp = NULL;
		}
	}

	vSocket.empty();


	system("PAUSE");
	return 0;
}
