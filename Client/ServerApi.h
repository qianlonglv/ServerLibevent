#ifndef _SERVERAPI_H
#define _SERVERAPI_H

#include <string>
using namespace std;

extern "C" _declspec(dllimport) int ConnectServer(SOCKET &sock, char *sIP, int iPort);
extern "C" _declspec(dllimport) int disConnectServer(SOCKET &sock);
extern "C" _declspec(dllimport) int getString(SOCKET sock, char *sContent, int iLen);
extern "C" _declspec(dllimport) int setString(SOCKET sock, char *sSendData, int iLen);


#endif