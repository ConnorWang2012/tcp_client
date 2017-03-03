#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

#ifdef _WIN32
#include <WinSock2.h>
#endif

#include "event2/bufferevent.h"
#include "event2/event.h"

static void event_cb(struct bufferevent* bev, short event, void* ctx);

static void read_cb(struct bufferevent* bev, void* ctx);

static void write_cb(struct bufferevent* bev, void* ctx);


int _tmain(int argc, _TCHAR* argv[])
{
	struct event_base* base;
	struct bufferevent* bev;
	struct sockaddr_in sin;
	const char* ip = "127.0.0.1";
	int port = 9876;

#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

	base = event_base_new();
	if (!base) {
		perror("event_base_new failed!");
		return 1;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(ip);
	sin.sin_port = htons(port);

	bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		perror("bufferevent_socket_new failed!");
		return 1;
	}

	bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);
	bufferevent_enable(bev, EV_READ | EV_WRITE);
	if (-1 == bufferevent_socket_connect(bev, (struct sockaddr*)&sin, sizeof(sin))) {
		perror("bufferevent_socket_connect failed!");
		return 1;
	}

	event_base_dispatch(base);
	
	return 0;
}


static void event_cb(struct bufferevent* bev, short event, void* ctx) {
	if (event & BEV_EVENT_ERROR) {
		printf("Error from bufferevent!");
	}

	if (event & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
		bufferevent_free(bev);
	}

	if (event & BEV_EVENT_CONNECTED) {
		printf("client connected");
	} else if (event & BEV_EVENT_TIMEOUT) {
		printf("client connect timeout");
	}
}

static void read_cb(struct bufferevent* bev, void* ctx) {
	// This callback is invoked when there is data to read on bev.
	struct evbuffer* input = bufferevent_get_input(bev);
	struct evbuffer* output = bufferevent_get_output(bev);
}

static void write_cb(struct bufferevent* bev, void* ctx) {

}