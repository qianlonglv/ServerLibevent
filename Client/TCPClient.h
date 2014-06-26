
SOCKET connectServer(const char *hostip, u_short port);
void connectionClose(SOCKET s);
int SendtoServer(SOCKET s, const char *buf, int buflen);
int RecvFromServer(SOCKET s, char *buf, int buflen);
