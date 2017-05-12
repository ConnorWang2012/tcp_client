#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "src/network_manager.h"

int main(int argc, char* argv[])
{
	gamer::NetworkManager::getInstance()->connect();
	//gamer::NetworkManager::getInstance()->disconnect();
	//gamer::NetworkManager::getInstance()->connect();
	
	system("pause");
	return 0;
}