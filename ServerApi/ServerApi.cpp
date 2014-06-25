// ServerApi.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ServerApi.h"
#include <mmsystem.h>

#pragma comment( lib,"winmm.lib" )
#pragma comment(lib,"ws2_32.lib")

#define SAFE_DEL_SOCKET(s)      { if (s != INVALID_SOCKET) { closesocket(s); s = INVALID_SOCKET;}}


int
WriteData(SOCKET sock, LPVOID lpData, int iLen)
{
	fd_set fdsend;
	timeval timeout = {0, 500};
	int iPos = 0;
	while (iPos < iLen)
	{
		FD_ZERO(&fdsend);
		FD_SET(sock, &fdsend);
		select(NULL, NULL, &fdsend, NULL, &timeout);
		if (!FD_ISSET(sock, &fdsend))
		{
			continue;
		}
		int iSend = send(sock, (LPCSTR)lpData + iPos, iLen - iPos, 0);
		if (iSend <= 0) 
		{
			//TRACE("Error CGeTCP::WriteData, errorcode: %d\n", WSAGetLastError());
			//SAFE_DEL_SOCKET(m_hSock);
			break;
		}
		iPos += iSend;
	}

	return iPos;
}

int
ReadData(SOCKET sock, char *pBuffer, int iReadLen)
{
	DWORD dwCurTm = timeGetTime();
	int iRecvLen = 0;


	fd_set fdRecv;
	FD_ZERO(&fdRecv);
	FD_SET(sock, &fdRecv);
	timeval timeout = {5, 0};
	int iRet = select(0, &fdRecv, NULL, NULL, &timeout);
	if (iRet < 0)
	{
		return WSAGetLastError();
	}

	if (!FD_ISSET(sock, &fdRecv))
	{
		if (timeGetTime() > dwCurTm + 1000)
		{
			return FALSE;
		}
	}

	iRecvLen = recv(sock, pBuffer, iReadLen, 0);
	if (iRecvLen < 0)
	{
		return WSAGetLastError();
	}

	return TRUE;
}

int ConnectServer(SOCKET &sock, string sIp,	int iPort)
{
	if (sock != INVALID_SOCKET || sIp == "" || iPort == 0)
	{
		return FALSE;
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	SOCKADDR_IN sevAddr = {};
	sevAddr.sin_family = AF_INET;
	sevAddr.sin_port = htons(iPort);
	sevAddr.sin_addr.s_addr = inet_addr(sIp.c_str());
	
	if (connect(sock, (SOCKADDR *)&sevAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		SAFE_DEL_SOCKET(sock);
		return WSAGetLastError();
	}
	
	return TRUE;
}

int disConnectServer(SOCKET &sock)
{
	SAFE_DEL_SOCKET(sock);
	return TRUE;
}

int getString(SOCKET sock, string &sContent)
{
	if (WriteData(sock, "GET", strlen("GET")) > 0)
	{
		char pbuffer[10] = {};
		ReadData(sock, pbuffer, 10);
		sContent = pbuffer;

		return TRUE;
	}
	
	return FALSE;
}

int setString(SOCKET sock, string sSendData)
{
	if (sock != INVALID_SOCKET && sSendData != "")
	{
		return WriteData(sock, (LPVOID)sSendData.c_str(), sSendData.size());
	}

	return FALSE;
}