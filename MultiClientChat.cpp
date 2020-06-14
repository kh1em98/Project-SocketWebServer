#pragma once
#include "MultiClientChat.h"
#include <string>
using namespace std; 

void MultiClientChat::onClientConnected(int clientSocket)
{
	string welcomeMsg = "Dit con me may cut\n";
	sendToClient(clientSocket, welcomeMsg.data(), welcomeMsg.length() + 1);
}

void MultiClientChat::onClientDisconnected(int clientSocket)
{

}

void MultiClientChat::onMessageReceived(int clientSocket, const char* msg, int length)
{
	broadcastToClients(clientSocket, msg, length);
}
