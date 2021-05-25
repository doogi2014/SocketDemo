
#include "zh_socket.h"

#include "atlbase.h"
#include "atlstr.h"
#include <ctime>
#include <io.h>

DWORD CALLBACK accept_thread(void* _p);
DWORD CALLBACK client_thread(void* _p);

void zh_socket::stop()
{
	
	quit = true;
	if (listenfd) {
		closesocket(listenfd);
	}

	WSACleanup();
	printf("socket stop\r\n");
}

int zh_socket::start(const char* ip, int port, ClientConnectCallback cc)
{
	this->ccCallback = cc;

	WSADATA d;
	WSAStartup(0x0202, &d);

	listenfd = -1;
	quit = false;

	printf("socket start\r\n");
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
		return -1;
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	addr.sin_port = htons(port);

	if (bind(listenfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
		closesocket(listenfd);
		return -1;
	}

	listen(listenfd, 5);

	HANDLE h1 = CreateThread(NULL, 0, accept_thread, this, 0, NULL);
	if (h1)
		CloseHandle(h1);
	return 0;
}

DWORD CALLBACK accept_thread(void* _p)
{
	zh_socket* ws = (zh_socket*)_p;

	printf("accept_thread start!\r\n");
	while (!ws->quit) {

		fd_set rdst; 
		FD_ZERO(&rdst);  
		FD_SET(ws->listenfd, &rdst);
		timeval tmo; 
		tmo.tv_sec = 1; 
		tmo.tv_usec = 0;
		int r = select(0, &rdst, NULL, NULL, &tmo);

		if (r <= 0)
			continue;

		int sock = accept(ws->listenfd, NULL, NULL); 
		if (sock < 0)
			continue;

		int tcp_nodelay = 1;
		setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&tcp_nodelay, sizeof(int));

		ws->socks.push_back(sock);
		// 当有新的连接时，执行回调
		if (ws->ccCallback != NULL)
			ws->ccCallback();
	
		printf("new client:%d  socks.size:%d\r\n", sock,ws->socks.size());
		HANDLE h2 = CreateThread(NULL, 0, client_thread, ws, 0, NULL);
		if (h2)
			CloseHandle(h2);
	}

	printf("accept_thread stop!\r\n");
	return 0;
}

void del_sock(vector<int>* socks, int val)
{
	for (vector<int>::iterator it = socks->begin(); it != socks->end();) {
		if (*it == val) {
			it = socks->erase(it);
		}
		else {
			++it;
		}
	}
}

DWORD CALLBACK client_thread(void* _p)
{
	zh_socket* ws = (zh_socket*)_p;
	int sock = ws->socks.back();

	char buffer[8192];
	bool quit = false;

	while (!quit) {
		int r = recv(sock, buffer, sizeof(buffer), 0);
		if (r <= 0) {
			quit = true;
			break;
		}
		if (strstr(buffer, "EXIT")) {
			quit = true;
			break;
		}
		// do something when received data from client.
		printf("received count:%d\r\n", r);
	}

	int r = closesocket(sock);
	del_sock(&ws->socks, sock);
	printf("client_thread close:%d  socks.size:%d \r\n", sock, ws->socks.size());

	return 0;
}

void zh_socket::send_data(const char* buffer, int len)
{
	for (vector<int>::iterator it = socks.begin(); it != socks.end(); it++) {

		char buf[8] = { 0x31,0x39,0x38,0x35 };
		memcpy(buf + 4, &len, 4);
		send(*it, buf, 8, 0);
		send(*it, buffer, len, 0);
	}
}

