#include "network_manager.h"

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#elif defined(__APPLE__)
#endif

#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/event.h"

namespace gamer
{

NetworkManager::NetworkManager()
{

}

NetworkManager::NetworkManager(const std::string& ip, int port)
	:ip_(ip)
	,port_(port)
{

}

NetworkManager* NetworkManager::getInstance()
{
	static NetworkManager s_network_mgr; 
	return &s_network_mgr;
}

void NetworkManager::set_ip(const std::string& ip)
{
	ip_ = ip;
}

const std::string& NetworkManager::ip() const
{
	return ip_;
}

void NetworkManager::set_port(int port)
{
	port_ = port;
}

int NetworkManager::port() const
{
	return port_;
}

void NetworkManager::connect()
{
	this->initIPAndPort();

	if (evbase_ && bev_)
	{
		auto ret = event_base_loopcontinue(evbase_);
		printf("[NetworkManager::connect] ret : %d", ret);
		return;
	}

	struct sockaddr_in sin;

#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

	if (nullptr == evbase_)
	{
		evbase_ = event_base_new();
	}

	if (nullptr == evbase_)
	{
		perror("event_base_new failed!");
		return;
		// TODO : dispatch event_base_new failed event
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(ip_.c_str());
	//sin.sin_addr.s_addr = inet_pton(AF_INET, ip_.c_str(), &sin);
	sin.sin_port = htons(port_);

	if (nullptr == bev_)
	{
		bev_ = bufferevent_socket_new(evbase_, -1, BEV_OPT_CLOSE_ON_FREE);
	}

	if (nullptr == bev_)
	{
		perror("bufferevent_socket_new failed!");
		return;
		// TODO : dispatch bufferevent_socket_new failed event
	}

	bufferevent_setcb(bev_,
		&gamer::NetworkManager::onBuffereventRead,
		&gamer::NetworkManager::onBuffereventWrite,
		&gamer::NetworkManager::onBuffereventArrive,
		NULL);

	bufferevent_enable(static_cast<bufferevent*>(bev_), EV_READ | EV_WRITE | EV_PERSIST);

	if (-1 == bufferevent_socket_connect(bev_, (struct sockaddr*)&sin, sizeof(sin)))
	{
		perror("bufferevent_socket_connect failed!");
		return;
		// TODO : dispatch bufferevent_socket_connect failed event
	}

	//event_base_dispatch(evbase_);
	event_base_loop(evbase_, EVLOOP_NONBLOCK | EVLOOP_NO_EXIT_ON_EMPTY);
}

void NetworkManager::disconnect()
{
	if (evbase_)
	{
		auto ret = event_base_loopexit(evbase_, NULL);
		printf("[NetworkManager::disconnect] ret : %d", ret);
	}
}

void NetworkManager::send(void* ctx, size_t ctxlen, const ResponeCallback& cb)
{
	if (nullptr == ctx)
	{
		printf("[NetworkManager::send] context invalid");
		return;
	}

	if (ctxlen > NetworkManager::MAX_MSG_LEN)
	{
		printf("[NetworkManager::send] context len invalid");
		return;
	}

	bufferevent_write(bev_, ctx, ctxlen);
	request_callback_ = cb;
}

void NetworkManager::onBuffereventArrive(struct bufferevent* bev, short event, void* ctx) 
{
	if (event & BEV_EVENT_ERROR) 
	{
		printf("error from bufferevent!");
		// TODO : dispatch error from bufferevent event
	}

	if (event & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) 
	{
		bufferevent_free(bev);
		printf("error from bufferevent, free bufferevent!");
		// TODO : dispatch error from bufferevent, free bufferevent event
	}

	if (event & BEV_EVENT_CONNECTED) 
	{
		printf("client connected\n");
		// TODO : dispatch client connected success event

		//write_cb(bev, NULL);
		//char msg[] = "client connected";
		//bufferevent_write(bev, msg, sizeof(msg));
		auto input = bufferevent_get_input(bev);
		auto output = bufferevent_get_output(bev);
		//evbuffer_add_printf(output, "client msg : %s", "client connected 2");
		char msg[] = "login";
		gamer::NetworkManager::getInstance()->send(msg, sizeof(msg), [&](short i) {});
	} 
	else if (event & BEV_EVENT_TIMEOUT) 
	{
		printf("client connect timeout");
		// TODO : dispatch client connect timeout event
	}
}

void NetworkManager::onBuffereventRead(struct bufferevent* bev, void* ctx) 
{
	// This callback is invoked when there is data to read on bev.
	auto input = bufferevent_get_input(bev);
	auto output = bufferevent_get_output(bev);

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

void NetworkManager::initIPAndPort()
{
	ip_ = "127.0.0.1";
	port_ = 4994;
}

} // namespace gamer