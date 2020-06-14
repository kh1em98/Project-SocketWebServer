#pragma once

#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <sstream>
#pragma comment (lib, "ws2_32.lib")

using namespace std; 

class TcpListener
{

public:

	TcpListener(const char* ipAddress, int port) :
		m_ipAddress(ipAddress), m_port(port) { }

	// Initialize the listener
	int init();

	// Run the listener
	int run();

protected:
	// Handler for client connections
	virtual void onClientConnected(int clientSocket);

	virtual void onClientDisconnected(int clientSocket); 

	// Xu ly khi client nhan duoc message
	virtual void onMessageReceived(int clientSocket, const char* msg, int length);

	void sendToClient(int clientSocket, const char* msg, int length);

	void broadcastToClients(int sendingClient, const char* msg, int length);

private:
	const char*		m_ipAddress;	// IP Address server will run on
	int				m_port;			// Port # for the web service
	int				m_socket;		// Internal FD for the listening socket
	fd_set			m_master;		// Deo hieu la cai lon gi luon
};