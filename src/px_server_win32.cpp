#include "stdafx.h"
#include "px_server.h"
#include "px_request_impl.h"
#include "px_response_impl.h"

#ifdef _MSC_VER

namespace px_server
{
	bool PxServer::run(int port)
	{
		SOCKET sockSrv = startup(port);
		if (sockSrv == -1)
			return false;

		printf("Listen on port : %d\n", port);
		SOCKADDR_IN addrClient;
		int len = sizeof(SOCKADDR);
		m_running = true;
		while (m_running)
		{
			SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
			if (sockConn == SOCKET_ERROR)
			{
				printf("Accept failed:%d", WSAGetLastError());
				break;
			}

			PxRequestImpl* request = PxRequestImpl::allocWithSocket(sockConn);
			if (request == NULL)
				goto finished;
			PxResponseImpl* response = PxResponseImpl::allocWithSocket(sockConn);

			query(request, response);

			delete request, response;
		finished:
			closesocket(sockConn);
		}
		closesocket(sockSrv);
		WSACleanup();
		return true;
	}

	SOCKET PxServer::startup(int port)
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			return invalidSocket;

		SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

		SOCKADDR_IN addrSrv;
		addrSrv.sin_family = AF_INET;
		addrSrv.sin_port = htons((u_short)port);
		addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

		int retVal = bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
		if (retVal == SOCKET_ERROR)
		{
			printf("%s(%s): failed bind, error code %d\n", __FILE__, __FUNCTION__, WSAGetLastError());
			WSACleanup();
			return invalidSocket;
		}
		if (listen(sockSrv, 10) == SOCKET_ERROR)
		{
			printf("%s(%s): listen failed, error code %d\n", __FILE__, __FUNCTION__, WSAGetLastError());
			WSACleanup();
			return invalidSocket;
		}
		return sockSrv;
	}
}

#endif // _MSC_VER