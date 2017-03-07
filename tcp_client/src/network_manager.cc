
#include "network_manager.h"

#ifdef _WIN32
#include <WinSock2.h>
#endif

namespace gamer
{
NetworkManager* NetworkManager::s_shared_network_manager_ = nullptr;

NetworkManager::NetworkManager()
{

}

NetworkManager::~NetworkManager()
{

}

NetworkManager* NetworkManager::getInstance()
{
	if (nullptr == s_shared_network_manager_)
	{
		s_shared_network_manager_ = new NetworkManager();
		if( !s_shared_network_manager_->init() )
		{
			//SAFE_DELETE(s_shared_network_manager_);
			//CCLOG("event_manager init failed!");
	    }
	}
	return s_shared_network_manager_;
}

void NetworkManager::destoryInstance()
{
	//SAFE_DELETE(s_shared_network_manager_);
}

void NetworkManager::initSocket()
{
	initIPAndPort();

	struct event_base* base;
	struct bufferevent* bev;
	struct sockaddr_in sin;

#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

	base = event_base_new();
	if (!base) 
	{
		perror("event_base_new failed!");
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(ip_.c_str());
	sin.sin_port = htons(port_);

	bev = (bufferevent*)bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) 
	{
		perror("bufferevent_socket_new failed!");
	}
	
	bufferevent_setcb(bev, 
		&gamer::NetworkManager::onBuffereventRead, 
		&gamer::NetworkManager::onBuffereventWrite, 
		&gamer::NetworkManager::onBuffereventArrive, NULL);

	bufferevent_enable((bufferevent*)bev, EV_READ | EV_WRITE | EV_PERSIST);

	if (-1 == bufferevent_socket_connect(bev, (struct sockaddr*)&sin, sizeof(sin))) 
	{
		perror("bufferevent_socket_connect failed!");
	}

	//event_base_dispatch(base);
	event_base_loop(base, EVLOOP_NONBLOCK);
}

void NetworkManager::onBuffereventArrive(struct bufferevent* bev, short event, void* ctx) 
{
	if (event & BEV_EVENT_ERROR) 
	{
		printf("error from bufferevent!");
	}

	if (event & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) 
	{
		bufferevent_free(bev);
	}

	if (event & BEV_EVENT_CONNECTED) 
	{
		printf("client connected\n");
		//write_cb(bev, NULL);
		//char msg[] = "client connected";
		//bufferevent_write(bev, msg, sizeof(msg));
		struct evbuffer* input = bufferevent_get_input(bev);
		struct evbuffer* output = bufferevent_get_output(bev);
		evbuffer_add_printf(output, "client msg : %s", "client connected");
	} 
	else if (event & BEV_EVENT_TIMEOUT) 
	{
		printf("client connect timeout");
	}
}

void NetworkManager::onBuffereventRead(struct bufferevent* bev, void* ctx) 
{
	// This callback is invoked when there is data to read on bev.
	struct evbuffer* input = bufferevent_get_input(bev);
	struct evbuffer* output = bufferevent_get_output(bev);

	//if (my_n < 5) {
	//	my_n++;
		char buf[4096] = { 0 };
		size_t n = evbuffer_get_length(input);
		//if (evbuffer_remove(input, buf, n) > 0 ) { // read and remove
		if (evbuffer_copyout(input, buf, n) > 0 ) {  // read only
			printf("read data from server : %s\n", buf);
			//evbuffer_add_printf(output, "client msg %d", my_n);
		}
	//}
}

void NetworkManager::onBuffereventWrite(struct bufferevent* bev, void* ctx) 
{
	//printf("client read_cb");
	//char msg[] = "client msg";
	//bufferevent_write(bev, msg, sizeof(msg));
}

bool NetworkManager::init()
{
	return true;
}

void NetworkManager::initIPAndPort()
{
	ip_ = "127.0.0.1";
	port_ = 9876;
}

} // namespace gamer