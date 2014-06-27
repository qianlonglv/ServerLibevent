#ifndef _SERVERAPI_H
#define _SERVERAPI_H

#include "stdafx.h"
#include <string>

using namespace std;


extern "C" _declspec(dllexport) int ConnectServer(SOCKET &sock, char *sIP, int iPort);
extern "C" _declspec(dllexport) int disConnectServer(SOCKET &sock);
extern "C" _declspec(dllexport) int getString(SOCKET sock, char *sContent, int iLen);
extern "C" _declspec(dllexport) int setString(SOCKET sock, char *sSendData, int iLen);


#endif