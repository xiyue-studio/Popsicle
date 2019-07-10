#include "stdafx.h"
#include "px_server.h"
#include "px_response_impl.h"
#include "px_request_impl.h"

#ifdef  __GNUC__ // for gcc on Linux/Apple OS X

#include <sys/epoll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

namespace px_server
{
	bool PxServer::run(int port)
	{
		int epfd = epoll_create(10);
		if (epfd == -1)
		{
			printf("epoll_create error, errMsg:%s\n", strerror(errno));
			return false;
		}
		SOCKET listenfd = startup(port);
		if (listenfd <= 0)
		{
			printf("startup error at %s:%d\n", __FILE__, __LINE__);
			return false;
		}
		struct epoll_event event;
		event.data.fd = listenfd;
		event.events = EPOLLIN;

		int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);
		if (ret == -1)
		{
			close(epfd);
			return false;
		}
		struct epoll_event recvEvents[100];
		m_running = true;
		printf("Server is listening on %d\n", port);
		while (m_running)
		{
			int activityEventCount = epoll_wait(epfd, recvEvents, 100, -1);
			for (int eventIndex = 0; eventIndex < activityEventCount; eventIndex++)
			{
				int sock = recvEvents[eventIndex].data.fd;
				if (sock == listenfd && (recvEvents[eventIndex].events & EPOLLIN))
				{
					struct sockaddr_in client;
					socklen_t len = sizeof(client);

					SOCKET newSock = accept(listenfd, (struct sockaddr*)&client, &len);
					if (newSock < 0)
						continue;
					event.events = EPOLLIN;
					event.data.fd = newSock;
					epoll_ctl(epfd, EPOLL_CTL_ADD, newSock, &event);
				}
				else
				{
					if (recvEvents[eventIndex].events & EPOLLIN)
					{
						PxRequestImpl* request = PxRequestImpl::allocWithSocket(sock);
						if (request == NULL)
						{
							epoll_ctl(epfd, EPOLL_CTL_DEL, sock, NULL);
							close(sock);
						}
						PxResponseImpl* response = PxResponseImpl::allocWithSocket(sock);

						query(request, response);

						delete request;
						delete response;
						close(sock);
					}
					else if (recvEvents[eventIndex].events & EPOLLOUT)
					{
					}
					else
					{
						continue;
					}
				}
			}
		}
		return true;
	}

	SOCKET PxServer::startup(int port)
	{
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		struct sockaddr_in myAddr;
		bzero(&myAddr, sizeof(myAddr));
		myAddr.sin_family = AF_INET;
		myAddr.sin_port = htons(port);
		myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		bind(sockfd, (struct sockaddr*)&myAddr, sizeof(myAddr));

		listen(sockfd, 10);

		return sockfd;
	}
}

#endif //  __GNUC__ // for gcc on Linux/Apple OS X
