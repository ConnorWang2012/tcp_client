//#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "src/network_manager.h"

int main(int argc, char* argv[])
{
	gamer::NetworkManager::getInstance()->initSocket();
	return 0;
}