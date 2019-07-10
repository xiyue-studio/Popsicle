#pragma once

#include "px_server.h"
#include <map>
#include <string>

namespace px_server
{
	class PxResponseImpl : public PxResponse
	{
	public:
		static PxResponseImpl* allocWithSocket(SOCKET sock);

		virtual forceinline void setStatusCode(int code) { m_statusCode = code; }

		virtual void addHeaderField(const char* name, const char *valueFmt, ...);
		virtual void writeBody(const char* buffer, int dataSz);
	
	private:
		PxResponseImpl() {};
	public:
		virtual ~PxResponseImpl() {};

		int sendHeaderField(size_t contentLength);

		SOCKET m_sock;
		int m_statusCode;
		std::map<std::string, std::string, StringLess> m_headers;
	};
}
