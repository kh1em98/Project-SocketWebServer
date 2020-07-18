#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>
#include "WebServer.h"

using namespace std; 

int WebServer::init()
{
	// Khoi tao listening socket 
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		return WSAGetLastError();
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(m_port);
	inet_pton(AF_INET, m_ipAddress, &hint.sin_addr);

	if (bind(m_socket, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}

	// Tell Winsock the socket is for listening 
	if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}

	// Tao master file, gan tat ca bit = 0
	FD_ZERO(&m_master);

	FD_SET(m_socket, &m_master);
	return 0;
}

int WebServer::run()
{
	// this will be changed by the \quit command (see below, bonus not in video!)
	bool running = true;

	while (running)
	{

		// Make a copy of the master file descriptor set, this is SUPER important because
		// the call to select() is _DESTRUCTIVE_. The copy only contains the sockets that
		// are accepting inbound connection requests OR messages. 

		// E.g. You have a server and it's master file descriptor set contains 5 items;
		// the listening socket and four clients. When you pass this set into select(), 
		// only the sockets that are interacting with the server are returned. Let's say
		// only one client is sending a message at that time. The contents of 'copy' will
		// be one socket. You will have LOST all the other sockets.

		// SO MAKE A COPY OF THE MASTER LIST TO PASS INTO select() !!!

		fd_set copy = m_master;
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];

			if (sock == m_socket)
			{
				SOCKET client = accept(m_socket, nullptr, nullptr);

				FD_SET(client, &m_master);

			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					closesocket(sock);
					FD_CLR(sock, &m_master);
				}
				else
				{
					onMessageReceived(sock, buf, bytesIn);
				}
			}
		}
	}

	// Remove the listening socket from the master file descriptor set and close it
	// to prevent anyone else trying to connect.
	FD_CLR(m_socket, &m_master);
	closesocket(m_socket);

	while (m_master.fd_count > 0)
	{
		// Get the socket number
		SOCKET sock = m_master.fd_array[0];

		// Remove it from the master file list and close the socket
		FD_CLR(sock, &m_master);
		closesocket(sock);
	}

	// Cleanup winsock
	WSACleanup();
	return 0;
}



void WebServer::sendToClient(int clientSocket, const char* msg, int length)
{
	send(clientSocket, msg, length, 0);
}

void WebServer::broadcastToClients(int sendingClient, const char* msg, int length)
{
	// Send message to other clients
	for (int i = 0; i < m_master.fd_count; i++)
	{
		SOCKET outSock = m_master.fd_array[i];
		if (outSock != m_socket && outSock != sendingClient)
		{
			sendToClient(outSock, msg, length);
		}
	}
}

string get200Response(string fileContent)
{
	ostringstream oss;
	oss << "HTTP/1.1 200 OK\r\n";
	oss << "Cache-Control: no-cache, private\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << fileContent.size() << "\r\n";
	oss << "\r\n";
	oss << fileContent;
	string result = oss.str();
	return result;
}

string get301Response(string fileContent)
{
	ostringstream oss;
	oss << "HTTP/1.1 301 Moved Permanently\r\n";
	oss << "Location: /info.html\r\n";
	oss << "Cache-Control: no-cache, private\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << fileContent.size() << "\r\n";
	oss << "\r\n";
	oss << fileContent;
	string result = oss.str();
	return result;
}

string get404Response(string fileContent)
{
	ostringstream oss;
	oss << "HTTP/1.1 404 Not Found\r\n";
	oss << "Cache-Control: no-cache, private\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << fileContent.size() << "\r\n";
	oss << "\r\n";
	oss << fileContent;
	string result = oss.str();
	return result;
}


string getResponseToClient(string filename, int statusCode)
{
	string pathFile = ".\\wwwroot/" + filename;
	std::ifstream f(pathFile);
	string fileContent; 
	if (f.good())
	{
		std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
		fileContent = str; 
	}
	else
	{
		statusCode = 404; 
	}
	
	if (statusCode == 200)
	{
		return get200Response(fileContent);
	}
	else if (statusCode == 301)
	{
		return get301Response(fileContent);
	}
	else
	{
		return get404Response(fileContent);
	}
}

void WebServer::onMessageReceived(int clientSocket, const char* msg, int length)
{
	cout << msg << endl; 
	std::istringstream iss(msg);

	// Tach message thanh cac tu don de de phan tich
	std::vector<std::string> parsed((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
	string response; 

	if (parsed[0] == "POST")
	{
		// Lay username va password
		string ttDangNhap = parsed[parsed.size() - 1];
		int foundAmp = ttDangNhap.find('&');
		int startUsername = ttDangNhap.find("=") + 1;
		string username = ttDangNhap.substr(startUsername, foundAmp - startUsername);
		string password = ttDangNhap.substr(foundAmp + 10, ttDangNhap.size() - foundAmp - 10);

		// Neu dang nhap dung
		if (username == "admin" && password == "admin")
		{
			// Redirect qua /info.html
			response = getResponseToClient("info.html", 301);
		}
		// Dang nhap sai
		else
		{
			// Tra ve status 404
			response = getResponseToClient("404.html", 404);
		}
	}
	else if (parsed[0] == "GET")
	{
		// Lay filename qua parsed[1], co dang /name.html
		string filename = parsed[1].substr(1);
		if (filename == "")
		{
			filename = "index.html";
		}
		response = getResponseToClient(filename, 200);

	}
	sendToClient(clientSocket, response.data(), response.size() + 1);
}
