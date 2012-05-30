/// \file waHttpClient.cpp
/// HTTP客户端类实现文件

#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "waEncode.h"
#include "waHttpClient.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {

/// \defgroup waHttpClient waHttpClient相关全局函数

/// \ingroup waHttpClient
/// \fn int tcp_request( const string &server, const int port, const string &request, string &response, const int timeout )
/// 发送TCP请求并取得回应内容
/// \param server 服务器IP
/// \param port 服务器端口
/// \param request 发送的TCP请求
/// \param response 服务器的回应内容
/// \param timeout 超时时长,单位为秒,为0不判断超时
/// \retval 0 执行成功
/// \retval 1 创建socket失败
/// \retval 2 无法连接服务器
/// \retval 3 发送请求失败
/// \retval 4 设置定时器失败或者连接超时
/// \retval 10 未知错误
int tcp_request( const string &server, const int port, const string &request,
	string &response, const int timeout ) 
{
	// init
	struct sockaddr_in sin;	 
	sin.sin_family = AF_INET;
	sin.sin_port = htons( port );
	sin.sin_addr.s_addr = inet_addr( server.c_str() );
	
	// create socket
	int fd;
	if ( (fd=socket(AF_INET,SOCK_STREAM,0)) < 0 )
		return 1;

	// connect
	if ( connect(fd,(struct sockaddr*)&sin,sizeof(sin)) < 0 ) {
		close( fd );
		return 2;
	}

	// send request
	if ( send(fd,request.c_str(),request.length(),0) < 0 ) {
		close( fd );
		return 3;
	}

	fd_set fds;
	struct timeval tv;

	// set timer
	if ( timeout > 0 ) {
		FD_ZERO( &fds );
		FD_SET( fd, &fds );
		tv.tv_sec = timeout;
		tv.tv_usec = 0;

		if ( select(fd+1,&fds,NULL,NULL,&tv) <= 0 ) {
			close( fd );
			return 4;
		}
	}

	if ( ( timeout>0 && FD_ISSET(fd,&fds) ) || timeout<=0 ) {
		// recv response
		int readed = 0;
		int buflen = 1024;
		char buff[1024];
		
		while ( (readed=recv(fd,buff,buflen-1,0)) > 0 ) {
			buff[readed] = '\0';
			response += buff;
		}

		close( fd );
		return 0;
	} else {
		close( fd );
		return 10;
	}
}

/// \ingroup waHttpClient
/// \fn string gethost_byname( const string &domain )
/// 根据服务器域名取得IP
/// \param domain 服务器域名（不包含"HTTP:://"头及任何'/'字符）
/// \return 执行成功返回服务器IP,否则返回空字符串
string gethost_byname( const string &domain ) {
	string ip;
	if ( domain != "" ) {
		struct hostent *he = gethostbyname( domain.c_str() );
		if ( he!=NULL && he->h_addr!=NULL )
			ip = inet_ntoa( *((struct in_addr*)he->h_addr) );
	}

	return ip;
}

/// \ingroup waHttpClient
/// \fn bool isip( const string &ipstr )
/// 判断字符串是否为有效IP
/// \param ipstr IP字符串
/// \retval true 有效
/// \retval false 无效
bool isip( const string &ipstr ) {
	
	struct in_addr addr;
	if ( inet_aton(ipstr.c_str(),&addr) != 0 )
		return true;
	else
		return false;
}

/// 设置指定的HTTP请求Header
/// \param name Header名称
/// \param value Header值,
void HttpClient::set_header( const string &name, const string &value ) {
	if ( name != "" )
		_sets[name] = value;
}

/// 设置HTTP请求Referer Header
/// \param referer Referer Header值
void HttpClient::set_referer( const string &referer ) {
	if ( referer != "" )
		set_header( "Referer", referer );
}

/// 设置HTTP请求Authorization Header
/// \param username 用户名
/// \param password 用户口令
void HttpClient::set_auth( const string &username, const string &password ) {
	if ( username != "" ) {
		string auth = username + ":" + password;
		auth = "Basic " + base64_encode( auth );
		set_header( "Authorization", auth );
	}
}

/// 设置HTTP请求Cookie Header
/// \param name Cookie名称
/// \param value Cookie值
void HttpClient::set_cookie( const string &name, const string &value ) {
	if ( name != "" ) {
		if ( _sets["Cookie"] != "" )
			_sets["Cookie"] += "; ";
		_sets["Cookie"] += ( uri_encode(name) + "=" + uri_encode(value) );
	}
}

/// 设置HTTP请求CGI参数
/// \param name CGI参数名称
/// \param value CGI参数值
void HttpClient::set_param( const string &name, const string &value ) {
	if ( name != "" ) {
		if ( _params != "" ) _params += "&";
		_params += ( uri_encode(name) + "=" + uri_encode(value) );
	}
}

/// 分析HTTP URL字符串
/// \param urlstr 请求URL
/// \param parsed_host 服务器主机名分析结果
/// \param parsed_host 服务器主机地址分析结果
/// \param parsed_url 请求URL分析结果
/// \param parsed_param 请求参数分析结果
/// \param parsed_port 服务器端口分析结果
void HttpClient::parse_url( const string &urlstr, string &parsed_host, string &parsed_addr,
	string &parsed_url, string &parsed_param, int &parsed_port )
{
	String url = urlstr;
	url.trim();
	if ( url == "" ) {
		_errno = ERROR_REQUEST_NULL;
		return;
	}

	// parse hostname and url
	unsigned int pos;
	parsed_host = "";
	parsed_url = url;
	if ( strncasecmp(url.c_str(),"HTTP://",7) == 0 ) {
		// http://...
		if ( (pos=url.find("/",7)) != url.npos ) {
			// http://hostname/...
			parsed_host = url.substr( 7, pos-7 );
			parsed_url = url.substr( pos );
		} else {
			// http://hostname
			parsed_host = url.substr( 7 );
			parsed_url = "/";
		}
	}

	// parse param
	parsed_param = "";
	if ( (pos=parsed_url.rfind("?")) != parsed_url.npos ) {
		// cgi?param
		parsed_param = parsed_url.substr( pos+1 );
		parsed_url = parsed_url.substr( 0, pos );
	}

	// parse port
	parsed_port = 80;
	if ( (pos=parsed_host.rfind(":")) != parsed_host.npos ) {
		// hostname:post
		parsed_port = stoi( parsed_host.substr(pos+1) );
		parsed_host = parsed_host.substr( 0, pos );
	}
	
	// parse addr
	if ( !isip(parsed_host) )
		parsed_addr = gethost_byname( parsed_host );
	else
		parsed_addr = parsed_host;
}
				   
/// 生成HTTP请求字符串
/// \param url 所请求的URL
/// \param params 所请求的URL的CGI参数
/// \param host 服务器域名(或者IP)
/// \param method 请求方法(GET或者POST)
/// \return 返回生成的HTTP请求字符串
string HttpClient::gen_httpreq( const string &url, const string &params, 
	const string &host, const string &method ) 
{
	string request;
	request.reserve( 512 );
	
	request += method + " " + url;
	if ( method!="POST" && params!="" )
		request += "?" + params;
	request += " HTTP/1.1" + HTTP_CRLF;
	
	request += "HOST: " + host + HTTP_CRLF;
	request += "Accept: */*" + HTTP_CRLF;
	request += "User-Agent: Mozilla/4.0 (compatible; WebAppLib HttpClient)" + HTTP_CRLF;
	request += "Pragma: no-cache" + HTTP_CRLF;
	request += "Cache-Control: no-cache" + HTTP_CRLF;
	
	map<string,string>::const_iterator i;
	for ( i=_sets.begin(); i!=_sets.end(); ++i ) {
		if ( i->first != "" )
			request += i->first + ": " + i->second + HTTP_CRLF;
	}

	request += "Connection: close" + HTTP_CRLF;

	if ( method == "POST" ) {
		// post data
		request += "Content-Type: application/x-www-form-urlencoded" + HTTP_CRLF;
		request += "Content-Length: " + itos(params.length()) + HTTP_CRLF;
		request += HTTP_CRLF;
		request += params + HTTP_CRLF;
	}

	request += HTTP_CRLF;
	return request;
}

/// 执行HTTP请求
/// \param url HTTP请求URL
/// \param server 服务器IP或者域名,为空字符串则根据参数1获得,默认为空字符串,
/// 若参数url,server都不包含服务器地址信息,则函数返回失败
/// \param port 服务器端口,默认为80
/// \param method HTTP请求Method,默认为"GET"
/// \param timeout HTTP请求超时时长,单位为秒,默认为5秒
/// \retval true 执行成功
/// \retval false 执行失败
bool HttpClient::request( const string &url, const string &host, const int port, 
	const string &method, const int timeout )
{
	_errno = ERROR_NULL;
	
	// parse host,port,url info
	string parsed_host, parsed_addr, parsed_url, parsed_param;
	int parsed_port;
	this->parse_url( url, parsed_host, parsed_addr, parsed_url, 
			   		 parsed_param, parsed_port );		
	
	// check params
	if ( parsed_param != "" ) {
		if ( _params != "" ) _params += "&";
		_params += parsed_param;
	}

	// check port
	if ( port != 80 ) parsed_port = port;
	
	// check host
	if ( host != "" ) {
		if ( !isip(parsed_host) ) {
			parsed_host = host;
			parsed_addr = gethost_byname( parsed_host );
		} else {
			parsed_addr = host;
		}
	}
	if ( parsed_addr == "" ) {
		_errno = ERROR_SERVERINFO_NULL;
		return false;
	}
	
	// generate request string
	_request = this->gen_httpreq( parsed_url, _params, parsed_host, method );
	// request
	int reqres = tcp_request( parsed_addr, parsed_port, _request, _response, timeout );
	if ( reqres != 0 ) {
		_errno = static_cast<error_msg>( reqres );
		return false;
	}
	
	// parse response
	if ( _response == "" ) {
		_errno = ERROR_RESPONSE_NULL;
		return false;
	}

	this->parse_response( _response );
	return true;
}

/// URL 是否有效
/// \param url HTTP请求URL
/// \param server 服务器IP,为空字符串则根据参数1获得,默认为空字符串,
/// 若参数url,server都不包含服务器地址信息,则函数返回失败
/// \param port 服务器端口,默认为80
/// \retval true URL有效
/// \retval false URL已失效
bool HttpClient::exist( const string &url, const string &server, 
	const int port ) 
{
	bool res = false;

	// check
	if ( this->request(url,server,port,"HEAD",5) && this->done() )
		res = true;
		
	// clear
	_status = "";
	_content = "";
	_gets.clear();
	
	return res;
}

/// 分析HTTP返回
/// \param response HTTP返回字符串
void HttpClient::parse_response( const string &response ) {
	// clear response status
	_status = "";
	_content = "";
	_gets.clear();

	// split header and body
	unsigned int pos;
	String head;
	String body;
	if ( (pos=response.find(DOUBLE_CRLF)) != response.npos ) {
		head = response.substr( 0, pos );
		body = response.substr( pos+4 );
	} else if ( (pos=response.find("\n\n")) != response.npos ) {
		head = response.substr( 0, pos );
		body = response.substr( pos+2 );
	} else {
		_errno = ERROR_RESPONSE_INVALID;
		return;
	}	
				
	// parse status
	String status;
	if ( (pos=head.find(HTTP_CRLF)) != head.npos ) {
		status = head.substr( 0, pos );
		head = head.substr( pos+2 );
		
		// HTTP/1.1 status_number description_string
		status.trim();
		_gets["HTTP_STATUS"] = status;
		if ( strncmp(status.c_str(),"HTTP/",5) == 0 ) {
			unsigned int b1, b2;
			if ( (b1=status.find(" "))!=status.npos
				 && (b2=status.find(" ",b1+1))!=status.npos )
				_status = status.substr( b1+1, b2-b1-1 );
		}
	}

	// http response status
	if ( _status.length()>1 && _status[0]!='2' )
		_errno = ERROR_HTTPSTATUS;
	
	// parse header
	String line, name, value;
	vector<String> hds = head.split( "\n" );
	for ( unsigned int i=0; i<hds.size(); ++i ) {
		line = hds[i];
		line.trim();
		
		// name: value
		if ( (pos=line.find(":")) != line.npos ) {
			name = line.substr( 0, pos );
			name.trim();
			value = line.substr( pos+1 );
			value.trim();
			
			if ( name != "" ) {
				if ( _gets[name] != "" )
					_gets[name] += "\n";
				_gets[name] += value;
			}
		}
	}
	
	// parse body
	if ( this->get_header("Transfer-Encoding") == "chunked" )
		_content = this->parse_chunked( body );
	else
		_content = body;
}

/// 获取指定的HTTP返回Header
/// \param name Header名称,
/// \return 成功返回Header值,否则返回空字符串
string HttpClient::get_header( const string &name ) {
	if ( name != "" )
		return _gets[name];
	else
		return string( "" );		
}

/// 获取HTTP返回Set-Cookie Header
/// \return 返回Cookie列表数组,每个元素为一个Cookie值
vector<String> HttpClient::get_cookie() {
	String ck = this->get_header( "Set-Cookie" );
	vector<String> cks = ck.split( "\n" );
	return cks;
}

/// 获取HTTP返回Header
/// \return HTTP返回Header字符串
string HttpClient::dump_header() {
	// status
	string header = ( this->get_header("HTTP_STATUS") + "\n" );
	
	// for multi header
	String headers;
	vector<String> headerlist;
	
	map<string,string>::const_iterator i;
	for ( i=_gets.begin(); i!=_gets.end(); ++i ) {
		if ( i->first!="" && i->first!="HTTP_STATUS" ) {
			if ( (i->second).find("\n") != (i->second).npos ) {
				headers = i->second;
				headerlist = headers.split( "\n" );
				for ( unsigned j=0; j<headerlist.size(); ++j )
					header += i->first + ": " + headerlist[j] + "\n";
			} else {
				header += i->first + ": " + i->second + "\n";
			}
		}
	}
	
	return header;
}

/// 执行HTTP请求是否成功
/// \retval true 成功
/// \retval false 失败
bool HttpClient::done() const {
	if ( _status.isnum() ) {
		int ret = stoi( _status );
		if ( ret>=100 && ret<300 )
			return true;
	}
	return false;
}

/// 清空所有设置及状态值
void HttpClient::clear() {
	// set
	_params = "";
	_sets.clear();
	
	// get
	_status = "";
	_content = "";
	_gets.clear();
}

/// 分析HTTP返回chunked类型content正文
/// \param chunkedstr chunked编码字符串
/// \return 返回解码字符串
string HttpClient::parse_chunked( const string &chunkedstr ) {
	char crlf[3] = "\x0D\x0A";
	unsigned int pos, lastpos;
	int size = 0;
	string hexstr;

	// location HTTP_CRLF		
	if ( (pos=chunkedstr.find(crlf)) != chunkedstr.npos ) {
		hexstr = chunkedstr.substr( 0, pos );
		size = stoi( hexstr, ios::hex );
	}
	
	string res;
	res.reserve( chunkedstr.length() );
	
	while ( size > 0 ) {
		// append to content
		res += chunkedstr.substr( pos+2, size );
		lastpos = pos+size+4;
		
		// location next HTTP_CRLF
		if ( (pos=chunkedstr.find(crlf,lastpos)) != chunkedstr.npos ) {
			hexstr = chunkedstr.substr( lastpos, pos-lastpos );
			size = stoi( hexstr, ios::hex );
		} else {
			break;
		}
	}
			
	return res;
}

/// 返回错误信息描述
/// \return 返回错误信息描述
string HttpClient::error() const {
	switch ( _errno ) {
		case ERROR_NULL :
			return "ERROR_NULL";
		case ERROR_CREATE_SOCKET :
			return "ERROR_CREATE_SOCKET";
		case ERROR_CONNECT_SERVER :
			return "ERROR_CONNECT_SERVER";
		case ERROR_SEND_REQUEST :
			return "ERROR_SEND_REQUEST";
		case ERROR_RESPONSE_TIMEDOUT :
			return "ERROR_RESPONSE_TIMEDOUT";
		case ERROR_SERVERINFO_NULL :
			return "ERROR_SERVERINFO_NULL";
		case ERROR_REQUEST_NULL :
			return "ERROR_REQUEST_NULL";
		case ERROR_RESPONSE_NULL :
			return "ERROR_RESPONSE_NULL";
		case ERROR_RESPONSE_INVALID :
			return "ERROR_RESPONSE_INVALID";
		case ERROR_HTTPSTATUS :
			return "ERROR_HTTPSTATUS:" + status();
		default : 
			return "ERROR_UNKNOWN";
	}
}

} // namespace

