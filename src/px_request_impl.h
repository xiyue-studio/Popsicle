#pragma once

#include "px_server.h"
#include "http-parser/http_parser.h"
#include <map>
#include <string>

namespace px_server
{
	class PxRequestImpl : public PxRequest
	{
	public:
		static PxRequestImpl* allocWithSocket(SOCKET sock);

		virtual forceinline HttpMethod httpMethod() { return (HttpMethod)m_method; }
		virtual const char* headerValueWithName(const char* name);
		virtual const char* parameterWithName(const char* name);
		virtual forceinline const char* documentUri() { return m_documentUri; };
		virtual forceinline size_t contentLength() { return m_content.length; }
		virtual forceinline const char* content() { return m_content.str; }

	private:
		static int _urlCallback(http_parser* parser, const char *at, size_t length);
		static int _headerFieldCallback(http_parser* parser, const char *at, size_t length);
		static int _headerValueCallback(http_parser* parser, const char *at, size_t length);
		static int _headersCompleteCallback(http_parser* parser);
		static int _bodyCallback(http_parser* parser, const char *at, size_t length);
		static int _statusCallback(http_parser*, const char *at, size_t length);
		static int _messageBegin(http_parser*);
		static int _messageComplete(http_parser*);

		bool httpParserParse();
		int recvRequest(SOCKET sock);

	private:
		PxRequestImpl();

	public:
		virtual ~PxRequestImpl();

	private:
		std::map<PxString, PxString> m_headers;
		char m_documentUri[512];

		char m_parameterPool[1024];
		char m_queryString[1024];

		PxString m_tempField;
		http_method m_method;

		PxString m_content;

		char* m_requestBuffer;
		int m_requestBufferSz;
	};
}
