#include "stdafx.h"
#include "px_server.h"
#include "px_request_impl.h"
#include "px_response_impl.h"

namespace px_server
{
	PxServer::PxServer()
	{
		m_running = false;
	}

	bool PxServer::query(PxRequest* request, PxResponse* response)
	{
		printf("%s\n", request->documentUri());
		printf("content-type:%s\n", request->headerValueWithName("content-type"));
		printf("Postman-Token:%s\n", request->headerValueWithName("Postman-Token"));
		printf("cache-control:%s\n", request->headerValueWithName("cache-control"));
		printf("HeaderField: %.*s\n", (int)request->contentLength(), request->content());

		response->addHeaderField("abcd", "ddd");
		int length = (int)strlen("hello-world");
 		response->addHeaderField("content-length", "%d", length);
 		response->writeBody("hello-world", length);
		return true;
	}
}
