
#ifndef CITY_HUNTER_SRC_NETWORK_MANAGER_H_
#define CITY_HUNTER_SRC_NETWORK_MANAGER_H_

#include <string>

struct bufferevent;
struct event_base;

namespace gamer
{

class NetworkManager
{
public:
	NetworkManager& operator=(const NetworkManager&) = delete;

	NetworkManager(const NetworkManager&) = delete;

	void *operator new(std::size_t) = delete;
	
	static NetworkManager* getInstance();

	void set_ip(const std::string& ip);

	const std::string& ip() const;

	void set_port(int port);

	int port() const;

	void connect();

	void disconnect();

private:
	NetworkManager();

	NetworkManager(const std::string& ip, int port);
	
	void initIPAndPort();

	//void disconnect();

	static void onBuffereventArrive(struct bufferevent* bev, short event, void* ctx);

	static void onBuffereventRead(struct bufferevent* bev, void* ctx);

	static void onBuffereventWrite(struct bufferevent* bev, void* ctx);

	struct event_base* evbase_;
	struct bufferevent* bev_;

	std::string ip_;
	int port_;
};

} // namespace gamer

#endif // CITY_HUNTER_SRC_NETWORK_MANAGER_H_