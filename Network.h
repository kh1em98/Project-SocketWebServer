#pragma once

#include <iostream>
#include <WinSock2.h>


using namespace std;

	class Network
	{
	public:
		static bool Initialize();
		static void Shutdown();
	};
