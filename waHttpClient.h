/// \file waHttpClient.h
/// HTTP客户端类头文件
/// 依赖于 webapp::String, webapp::Encode
/// <a href="wa_httpclient.html">使用说明文档及简单范例</a>

#ifndef _WEBAPPLIB_HTTPCLIENT_H_
#define _WEBAPPLIB_HTTPCLIENT_H_ 

#include <string>
#include <vector>
#include <map>
#include "waString.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {
	
const string HTTP_CRLF = "\r\n";
const string DOUBLE_CRLF = "\r\n\r\n";
	
/// 发送TCP请求并取得回应内容
int tcp_request( const string &server, const int port, const string &request, 
	string &response, const int timeout );
/// 根据服务器域名取得IP
string gethost_byname( const string &domain );
/// 判断字符串是否为有效IP
bool isip( const string &ipstr );

/// HTTP客户端类
/// <a href="wa_httpclient.html">使用说明文档及简单范例</a>
class HttpClient {
	public:
	
	/// \enum 错误信息
	enum error_msg {
		/// 无错误
		ERROR_NULL					= 0,
		/// 创建socket失败
		ERROR_CREATE_SOCKET			= 1,
		/// 无法连接服务器
		ERROR_CONNECT_SERVER		= 2,
		/// 发送请求失败
		ERROR_SEND_REQUEST			= 3,
		/// 设置定时器失败或者连接超时
		ERROR_RESPONSE_TIMEDOUT		= 4,
		/// 服务期地址信息错误
		ERROR_SERVERINFO_NULL		= 5,
		/// HTTP请求格式错误
		ERROR_REQUEST_NULL			= 6,
		/// 服务器回应为空
		ERROR_RESPONSE_NULL			= 7,
		/// 服务器回应格式错误
		ERROR_RESPONSE_INVALID		= 8,
		/// 服务器回应HTTP状态错误
		ERROR_HTTPSTATUS			= 9,
		/// 未知错误
		ERROR_UNKNOWN				= 10
	};

	/// 默认构造函数
	HttpClient(){};
	
	/// 构造并执行HTTP请求
	/// \param url HTTP请求URL
	/// \param server 服务器IP,为空字符串则根据参数1获得,默认为空字符串
	/// \param port 服务器端口,默认为80
	/// \param method HTTP请求Method,默认为"GET"
	/// \param timeout HTTP请求超时时长,单位为秒,默认为5秒,为0不判断超时
	HttpClient( const string &url, const string &server = "", const int port = 80, 
		const string &method = "GET", const int timeout = 5 ) 
	{
		this->request( url, server, port, method, timeout );
	}
		  
	/// 析构函数
	virtual ~HttpClient(){};

	/// 设置指定的HTTP请求Header
	void set_header( const string &name, const string &value );
	/// 设置HTTP请求Referer Header
	void set_referer( const string &referer );
	/// 设置HTTP请求Authorization Header
	void set_auth( const string &username, const string &password );
	/// 设置HTTP请求Cookie Header
	void set_cookie( const string &name, const string &value );
	/// 设置HTTP请求CGI参数
	void set_param( const string &name, const string &value );

	/// 执行HTTP请求
	bool request( const string &url, const string &server = "", const int port = 80, 
		const string &method = "GET", const int timeout = 5 );
	/// URL 是否有效
	bool exist( const string &url, const string &server = "", const int port = 80 );

	/// 获取指定的HTTP返回Header
	string get_header( const string &name );
	/// 获取HTTP返回Set-Cookie Header
	vector<String> get_cookie();
	/// 获取HTTP返回Header
	string dump_header();
	
	/// 执行HTTP请求是否成功
	bool done() const;
	/// 清空所有设置及状态值
	void clear();
	
	/// 获取HTTP返回Status
	/// \return HTTP返回Status字符串
	inline string status() const {
		return _status;
	}
	/// 获取HTTP返回Content正文
	/// \return HTTP返回Content正文
	inline string content() const {
		return _content;
	}
	/// 获取HTTP返回Content正文长度(Content-Length)
	/// \return HTTP返回Content正文长度
	inline size_t content_length() const {
		return _content.length();
	}
	
	/// 返回错误信息代码
	/// 代码信息定义参见 Http::error_msg
	inline error_msg errnum() const {
		return _errno;
	}
	/// 返回错误信息描述
	string error() const;

	/// 输出生成的HTTP请求全文
	/// \return 返回生成的HTTP请求全文
	inline string dump_request() const {
		return _request;
	}
	/// 输出获得的服务器返回全文
	/// \return 返回获得的服务器返回全文
	inline string dump_response() const {
		return _response;
	}
	
	////////////////////////////////////////////////////////////////////////////
	private:

	/// 分析HTTP URL字符串
	void parse_url( const string &url, string &parsed_host, string &parsed_addr,
		string &parsed_url, string &parsed_param, int &parsed_port );
	/// 生成HTTP请求字符串
	string gen_httpreq( const string &url, const string &params,
		const string &host, const string &method );
	/// 分析HTTP返回
	void parse_response( const string &response );
	/// 分析HTTP返回chunked类型content正文
	string parse_chunked( const string &chunkedstr );
	
	// set		
	String _request;			// generated request
	String _params;				// http request params
	map<string,string> _sets;	// push http headers

	// get
	String _response;			// server response
	String _status;				// http response status
	String _content;			// http response content
	map<string,string> _gets;	// recv http headers
	
	error_msg _errno;			// current error code
};

} // namespace

#endif //_WEBAPPLIB_HTTPCLIENT_H_ 

