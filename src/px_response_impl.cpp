#include "stdafx.h"
#include "px_server.h"
#include "http-parser/http_parser.h"
#include "px_response_impl.h"

using namespace std;
namespace px_server
{
	PxResponseImpl* PxResponseImpl::allocWithSocket(SOCKET sock)
	{
		PxResponseImpl* o = new PxResponseImpl();
		o->m_sock = sock;
		o->m_statusCode = HTTP_STATUS_OK;
		return o;
	}

	void PxResponseImpl::addHeaderField(const char* name, const char* valueFmt, ...)
	{
		char tmp[32] = { 0 };
		va_list ap;
		va_start(ap, valueFmt);
		sprintf(tmp, valueFmt, ap);
		va_end(ap);
		strcat(tmp, "\r\n");
		m_headers.insert(make_pair(std::string(name), std::string(tmp)));
	}

	void PxResponseImpl::writeBody(const char* buffer, int dataSz)
	{
		if (sendHeaderField(dataSz) > 0)
			send(m_sock, buffer, dataSz, 0);
	}

	int PxResponseImpl::sendHeaderField(size_t contentLength)
	{
		string strTmp = std::to_string(contentLength);
		strTmp.append("\r\n");
		m_headers["Content-Length"] = strTmp;
		char header[1024] = { 0 };

		char line[32];
		sprintf(line, "HTTP/1.1 %d %s\r\n", m_statusCode, http_status_str((http_status)m_statusCode));
		size_t cursor = strlen(line);
		strcat(header, line);
		map<string, string>::iterator iter = m_headers.begin();
		for (; iter != m_headers.end(); iter++)
		{
			strcat(header + cursor, iter->first.c_str());
			cursor += iter->first.length();
			header[cursor++] = ':';
			strcat(header + cursor, iter->second.c_str());
			cursor += iter->second.length();
		}
		strcat(header + cursor, "\r\n");
		cursor += 2;
		header[cursor] = '\0';

		return send(m_sock, header, (int)cursor, 0);
	}
}
