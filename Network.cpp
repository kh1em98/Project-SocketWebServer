#include "Network.h"

bool Network::Initialize()
{
	WSADATA wsadata;

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		cerr << "Khoi tao winsock that bai \n";
		return false;
	}

	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2)
	{
		cerr << "Sai phien ban winsock \n";
		return false;
	}

	return true;
}

void Network::Shutdown()
{
	WSACleanup();
}
