#include "Network.h"
#include "WebServer.h"

using namespace std; 

int main()
{
	Network::Initialize(); 
	WebServer webServer("127.0.0.1", 8080);
	webServer.init();
	webServer.run(); 
	Network::Shutdown(); 
}