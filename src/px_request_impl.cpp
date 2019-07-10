#include <stdlib.h>
#include "stdafx.h"
#include "px_request_impl.h"
#include "http-parser/http_parser.h"

using namespace std;

namespace px_server
{
	PxRequestImpl::PxRequestImpl()
	{
		m_headers.clear();
		m_parameterPool[0] = m_queryString[0] = m_documentUri[0] = '\0';

		m_method = HTTP_GET;

		m_requestBuffer = NULL;
		m_requestBufferSz = 0;
	}

	PxRequestImpl::~PxRequestImpl()
	{
		free(m_requestBuffer);
	}

	int PxRequestImpl::_urlCallback(http_parser* parser, const char* at, size_t length)
	{
		PxRequestImpl* request = (PxRequestImpl*)parser->data;
		if (request == NULL)
			return 1;

		if (length >= element_of(request->m_parameterPool))
			return 1;

		const char* parameter = strchr(at, '?') + 1;
		// /px?a=b&cc=4
		if (parameter > at)
		{
			strncpy(request->m_documentUri, at, parameter - at - 1);
			request->m_documentUri[parameter - at - 1] = '\0';
			strncpy(request->m_queryString, parameter, length - (parameter - at));
			request->m_queryString[length - (parameter - at)] = '\0';
		}
		else
		{
			strncpy(request->m_documentUri, at, length);
			request->m_documentUri[length] = '\0';
		}

		strcpy(request->m_parameterPool, request->m_queryString);
		for (size_t i = 0; request->m_parameterPool[i] != '\0'; i++)
		{
			if (request->m_parameterPool[i] == '&')
				request->m_parameterPool[i] = '\0';
		}
		return 0;
	}

	int PxRequestImpl::_headerFieldCallback(http_parser* parser, const char *at, size_t length)
	{
		// printf("HeaderField: %.*s\n", (int)length, at);
		PxRequestImpl* request = (PxRequestImpl*)parser->data;
		if (request == NULL)
			return 1;

		request->m_tempField.str = at;
		request->m_tempField.length = length;
		return 0;
	}

	int PxRequestImpl::_headerValueCallback(http_parser* parser, const char *at, size_t length)
	{
		// printf("HeaderValue: %.*s\n", (int)length, at);
		PxRequestImpl* request = (PxRequestImpl*)parser->data;
		if (request == NULL || request->m_tempField.length == 0)
			return 1;

		request->m_headers.insert(make_pair(request->m_tempField, PxString(at, length)));
		request->m_tempField.length = 0;
		request->m_requestBuffer[at - request->m_requestBuffer + length] = '\0';

		return 0;
	}

	int PxRequestImpl::_headersCompleteCallback(http_parser* parser)
	{
		PxRequestImpl* request = (PxRequestImpl*)parser->data;
		if (request == NULL)
			return 1;
		request->m_method = (http_method)parser->method;
		request->m_content.length = parser->content_length;
		return 0;
	}

	int PxRequestImpl::_bodyCallback(http_parser* parser, const char *at, size_t length)
	{
		// printf("Body: %.*s\n", (int)length, at);
		PxRequestImpl* request = (PxRequestImpl*)parser->data;
		if (request == NULL)
			return 1;

		request->m_content.str = at;
		if (request->m_content.length == 0)
		{
			request->m_content.length = length;
			request->m_requestBuffer[at - request->m_requestBuffer + length] = '\0';
		}
		return 0;
	}

	int PxRequestImpl::_statusCallback(http_parser*, const char *, size_t)
	{
		// printf("Body: %.*s\n", (int)length, at);
		return 0;
	}

	int PxRequestImpl::_messageBegin(http_parser*)
	{
		// printf("\n***MESSAGE Begin***\n\n");
		return 0;
	}

	int PxRequestImpl::_messageComplete(http_parser*)
	{
		// printf("\n***MESSAGE COMPLETE***\n\n");
		return 0;
	}

	PxRequestImpl* PxRequestImpl::allocWithSocket(SOCKET sock)
	{
		PxRequestImpl* o = new PxRequestImpl();
		if (o->recvRequest(sock) == 0)
		{
			delete o;
			o = NULL;
		}
		if (!o->httpParserParse())
		{
			delete o;
			o = NULL;
		}
		return o;
	}

	int PxRequestImpl::recvRequest(SOCKET sock)
	{
		m_requestBufferSz = 0;
		int recved = 0, i = 0;
		do
		{
			if (i == 0)
				m_requestBuffer = (char*)malloc(sizeof(char) * 1024);
			else
				m_requestBuffer = (char*)realloc(m_requestBuffer, 1024 * (i + 1));
			
			if (m_requestBuffer == NULL)
				break;
			recved = recv(sock, m_requestBuffer + 1024 * i, 1024 * sizeof(char), 0);
			if (recved < 0)
				break;

			m_requestBufferSz += recved;
			i++;
		} while (recved == 1024);

		if (recved < 0 || m_requestBuffer == NULL)
		{
			free(m_requestBuffer);
			m_requestBuffer = NULL;
			m_requestBufferSz = 0;
		}
		return m_requestBufferSz;
	}

	bool PxRequestImpl::httpParserParse()
	{
		if (m_requestBuffer == NULL || m_requestBufferSz == 0)
			return false;

		http_parser_settings settings;
		http_parser_settings_init(&settings);

		settings.on_url = _urlCallback;
		settings.on_header_field = _headerFieldCallback;
		settings.on_header_value = _headerValueCallback;
		settings.on_headers_complete = _headersCompleteCallback;
		settings.on_body = _bodyCallback;

		settings.on_message_begin = _messageBegin;
		settings.on_status = _statusCallback;
		settings.on_message_complete = _messageComplete;

		http_parser* parser = (http_parser*)malloc(sizeof(http_parser));
		http_parser_init(parser, HTTP_REQUEST);
		parser->data = this;
		size_t parsedLength = http_parser_execute(parser, &settings, m_requestBuffer, m_requestBufferSz);
		if (parser->upgrade)
		{
			/* handle new protocol */
		}
		else if (parsedLength != m_requestBufferSz)
		{
			/* Handle error. Usually just close the connection. */
		}
		if (parser->http_errno != 0)
		{
			printf("%s:%s\n", http_errno_name((http_errno)parser->http_errno),
				http_errno_description((http_errno)parser->http_errno));
			free(parser);
			return false;
		}
		free(parser);
		return true;
	}

	const char* PxRequestImpl::headerValueWithName(const char* name)
	{
		if (name == NULL)
			return NULL;

		map<PxString, PxString>::iterator iter = m_headers.find(PxString(name, strlen(name)));
		if (iter == m_headers.end())
			return NULL;
		return iter->second.str;
	}

	const char* PxRequestImpl::parameterWithName(const char* name)
	{
		if (name == NULL)
			return NULL;

		size_t nameLength = strlen(name);
		char* temp = (char*)malloc((nameLength + 2) * sizeof(char));

		strcpy(temp, name);
		temp[nameLength] = '=';
		temp[nameLength + 1] = '\0';

		char* p = strstr(m_queryString, temp);
		if (p == NULL)
			return NULL;
		size_t index = p - m_queryString;

		return &m_parameterPool[index + nameLength + 1];
	}
}
