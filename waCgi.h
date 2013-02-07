/// \file waCgi.h
/// webapp::Cgi,webapp::Cookie类头文件
/// 依赖于 webapp::String, webapp::Encode

#ifndef _WEBAPPLIB_CGI_H_
#define _WEBAPPLIB_CGI_H_ 

#include <string>
#include <map>

using namespace std;

/// Web Application Library namaspace
namespace webapp {

////////////////////////////////////////////////////////////////////////////////	
/// 输出HTML Content-Type header
void http_head();
/// 取得环境变量
string get_env( const string &envname );

////////////////////////////////////////////////////////////////////////////////

/// \defgroup waCgi waCgi相关数据类型与全局函数

/// \ingroup waCgi
/// \typedef CgiList 
/// Cgi 参数值列表类型 (map<string,string>)
typedef map<string,string> CgiList;

/// CGI参数读取类
class Cgi {
	public:

	/// 构造函数
	Cgi( const size_t formdata_maxsize = 0 );
	
	/// 析构函数
	virtual ~Cgi(){};
	
	/// 取得CGI参数
	string get_cgi( const string &name );
	
	/// 取得CGI参数
	inline string operator[] ( const string &name ) {
		return this->get_cgi( name );
	}
	
	/// FORM数据大小是否超出限制
	inline bool is_trunc() const {
		return _trunc;
	}
	
	/// 返回参数值列表
	/// \return 返回值类型为CgiList,即map<string,string>.	
	inline CgiList dump() const {
		return _cgi;
	}
	
	////////////////////////////////////////////////////////////////////////////
	private:
	
	/// 保存CGI参数
	void add_cgi( const string &name, const string &value );
	
	/// 分析urlencoded类型内容
	void parse_urlencoded( const string &buf );
	
	/// 分析multipart类型内容
	void parse_multipart( const string &content_type, const string &buf );

	map<string,string> _cgi;
	String _method;
	bool _trunc;
};

////////////////////////////////////////////////////////////////////////////
/// \ingroup waCgi
/// \typedef CookieList 
/// Cookie 参数值列表类型 (map<string,string>)
typedef map<string,string> CookieList;

/// Cookie读取,设置类
class Cookie {
	public:
	
	/// 构造函数
	Cookie();
	
	/// 析构函数
	virtual ~Cookie(){};
	
	/// 取得cookie内容
	string get_cookie( const string &name );
	
	/// 取得cookie内容
	inline string operator[] ( const string &name ) {
		return this->get_cookie( name );
	}
	
	/// 设置cookie内容
	void set_cookie( const string &name, const string &value, 
		const string &expires = "", const string &path = "/", 
		const string &domain = "" ) const;

	/// 清除指定的cookie内容
	/// \param name cookie名字
	inline void del_cookie( const string &name ) const {
		this->set_cookie( name, "", "Thursday,01-January-1970 08:00:01 GMT" );
	}
	
	/// 返回参数值列表
	/// \return 返回值类型为CookieList,即map<string,string>.	
	inline CookieList dump() const {
		return _cookies;
	}

	////////////////////////////////////////////////////////////////////////////
	private:
	
	/// 分析cookie内容
	void parse_cookie( const string &buf );

	map<string,string> _cookies;		
};

} // namespace

#endif //_WEBAPPLIB_CGI_H_

