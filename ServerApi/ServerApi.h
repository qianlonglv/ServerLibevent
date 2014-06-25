#ifndef _SERVERAPI_H
#define _SERVERAPI_H

#include "stdafx.h"
#include <string>

using namespace std;


extern "C" _declspec(dllexport) int ConnectServer(SOCKET &sock, string sIP, int iPort);
extern "C" _declspec(dllexport) int disConnectServer(SOCKET sock);
extern "C" _declspec(dllexport) int getString(SOCKET sock, string &sContent);
extern "C" _declspec(dllexport) int setString(SOCKET sock, string sSendData);


#endif