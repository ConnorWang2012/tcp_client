
#ifndef CITY_HUNTER_SRC_NETWORK_MANAGER_H_
#define CITY_HUNTER_SRC_NETWORK_MANAGER_H_

#include <string>

#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/event.h"

namespace gamer
{

class NetworkManager
{
public:
	~NetworkManager();

	static NetworkManager* getInstance();

	static void destoryInstance();

	void initSocket();

private:
	NetworkManager();

	bool init();
	
	void initIPAndPort();

	static void onBuffereventArrive(struct bufferevent* bev, short event, void* ctx);

	static void onBuffereventRead(struct bufferevent* bev, void* ctx);

	static void onBuffereventWrite(struct bufferevent* bev, void* ctx);

	static NetworkManager* s_shared_network_manager_;

	std::string ip_;
	int port_;
};

} // namespace gamer

#endif // CITY_HUNTER_SRC_NETWORK_MANAGER_H_
