
#pragma once

#include <WinSock.h>
#include <vector>
#include <string>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

typedef int(*ClientConnectCallback)();

class zh_socket
{
public:
	bool quit;
	int listenfd;
	vector<int> socks;
	ClientConnectCallback ccCallback;
	void* callbadkObj;

public:
	void stop();
	int start(const char* strip, int port, ClientConnectCallback cc);
	void send_data(const char* buffer, int len);
};


