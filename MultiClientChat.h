#pragma once
#include "TcpListener.h"

class MultiClientChat : public TcpListener
{
public:
	MultiClientChat(const char* ipAddress, int port) : TcpListener(ipAddress, port) {};
protected:
	virtual void onClientConnected(int clientSocket);

	virtual void onClientDisconnected(int clientSocket);

	// Xu ly khi client nhan duoc message
	virtual void onMessageReceived(int clientSocket, const char* msg, int length);

};