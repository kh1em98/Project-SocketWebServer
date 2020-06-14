#include "WebServer.h"

using namespace std; 

int main()
{
	WebServer webServer("0.0.0.0", 8080);
	if (webServer.init() != 0)
	{
		return 0; 
	}
	webServer.run(); 
}