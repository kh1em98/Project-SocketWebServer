#pragma once
#include <direct.h>
#define GetCurrentDir _getcwd
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>
#include "WebServer.h"

using namespace std; 


void WebServer::onClientConnected(int clientSocket)
{
}

void WebServer::onClientDisconnected(int clientSocket)
{
}

string responseToClient(string filename, int statusCode)
{
	string pathFile = ".\\wwwroot/" + filename;
	std::ifstream f(pathFile);
	string content; 
	int errorCode = 404; 
	if (f.good())
	{
		std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
		content = str; 
		errorCode = statusCode; 
	}
	
	std::ostringstream oss;

	oss << "HTTP/1.1 " << errorCode << " OK\r\n";
	oss << "Cache-Control: no-cache, private\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << content.size() << "\r\n";
	oss << "\r\n";
	oss << content;
	std::string output = oss.str();
	f.close(); 
	cout << output << endl;
	cout << "===========================================================" << endl; 
	return output;
}

void WebServer::onMessageReceived(int clientSocket, const char* msg, int length)
{
	std::istringstream iss(msg);
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
			response = responseToClient("info.html", 301);
		}
		// Dang nhap sai
		else
		{
			// Tra ve status 404
			response = responseToClient("404.html", 404);
		}
	}
	else if (parsed.size() >= 3 && parsed[0] == "GET")
	{
		response = responseToClient("index.html", 200);
	}

	sendToClient(clientSocket, response.data(), response.size() + 1);
}
