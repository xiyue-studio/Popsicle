#pragma once

namespace px_server
{
	enum HttpMethod
	{
		HttpMethod_delete = 0,
		HttpMethod_head,
		HttpMethod_get,
		HttpMethod_post,
		HttpMethod_put,
		HttpMethod_connect,
		HttpMethod_options,
		HttpMethod_trace,
		// ...
		HttpMethod_patch = 28,
		HttpMethod_max = 0xffffff
	};

	class PxRequest
	{
	public:
		virtual HttpMethod httpMethod() = 0;
		virtual const char* headerValueWithName(const char* name) = 0;
		virtual const char* parameterWithName(const char* name) = 0;
		virtual const char* documentUri() = 0;
		virtual size_t contentLength() = 0;
		virtual const char* content() = 0;

		virtual ~PxRequest() {};
	};

	class PxResponse
	{
	public:
		virtual void setStatusCode(int code) = 0;
		virtual void addHeaderField(const char* name, const char* valueFmt, ...) = 0;
		virtual void writeBody(const char* buffer, int dataSz) = 0;

		virtual ~PxResponse() {};
	};

	class PxServer
	{
	public:
		PxServer();
		virtual ~PxServer() {};

		bool run(int port);

	private:
		/** @brief
			@param [in] 
			@param [in]
			@return
				若返回 false 则表示没有处理请求，服务器将返回 404。
		*/
		virtual bool query(PxRequest* request, PxResponse* response);

		SOCKET startup(int port);

		bool m_running;
	};
}
