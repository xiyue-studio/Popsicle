#include "stdafx.h"
#include "px_server.h"

int main(int argc, const char* argv[])
{
	(void)argc;
	(void)argv;
	px_server::PxServer server;
	if (!server.run(8080))
	{
		printf("PxServer can not run!\n");
		return -1;
	}
	return 0;
}
