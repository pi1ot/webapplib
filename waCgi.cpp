/// \file waCgi.cpp
/// Cgi,Cookie类实现文件

#include <cstdlib>
#include <climits>
#include <iostream>
#include <vector>
#include "waString.h"
#include "waEncode.h"
#include "waCgi.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {
	
////////////////////////////////////////////////////////////////////////////////	

/// \ingroup waCgi
/// \fn void http_head()
/// 输出HTML Content-Type header,自动避免重复输出
void http_head() {
	static bool WEBAPP_ALREADY_HTTPHEAD = false;
	if ( !WEBAPP_ALREADY_HTTPHEAD ) {
		cout << "Content-Type: text/html\n" << endl;
		WEBAPP_ALREADY_HTTPHEAD = true;
	}
}

/// \ingroup waCgi
/// \fn string get_env( const string &envname )
/// 取得环境变量
/// \param envname 环境变量名
/// \return 成功返回环境变量值,否则返回空字符串
string get_env( const string &envname ) {
	const char *env = envname.c_str();
	char *val = getenv( env );
	if ( val != NULL ) 
		return string( val );
	else 
		return string( "" );
}

////////////////////////////////////////////////////////////////////////////
// CGI

/// 构造函数
/// 读取并分析CGI内容
/// \param formdata_maxsize 参数是"multipart/form-data"方式POST时的最大FORM上传数据大小,
/// 超过部分被截断不处理,单位为byte,默认为0即不限制数据大小
Cgi::Cgi( const size_t formdata_maxsize ) {
	// get envionment variable REQUEST_METHOD
	_method = get_env( "REQUEST_METHOD" );
	_method.upper();
	
	// set trunc flag
	_trunc = false;
	
	// method = GET
	if ( _method == "GET" ) {
		// read and parse QUERY_STRING
		this->parse_urlencoded( get_env("QUERY_STRING") );
	}
	
	// method = POST
	else if ( _method == "POST" ) {
		// get envionment variable CONTENT_TYPE
		string content_type = get_env( "CONTENT_TYPE" );
		
		char c;
		string buf;
		if ( formdata_maxsize > 0 )
			buf.reserve( formdata_maxsize );
		else
			buf.reserve( 256 );
		
		if ( content_type.find("application/x-www-form-urlencoded") != content_type.npos ) {
			// read stdin
			int content_length = atoi( (get_env("CONTENT_LENGTH")).c_str() );
			for ( int i=0; i<content_length; ++i ) {
				cin >> c;
				buf += c;
			}
			
			// parse stdin
			this->parse_urlencoded( buf );
			
		} else if ( content_type.find("multipart/form-data") != content_type.npos ) {
			// read stdin
			cin.unsetf( ios::skipws );

			if ( formdata_maxsize > 0 ) {
				// max size set
				size_t size = 0;
				
				while ( cin ) {
					cin >> c;
					buf += c;
					++size;
					
					if ( size > formdata_maxsize ) {
						cin.ignore( LONG_MAX );
						_trunc = true;
						break;
					}
				}
			} else {
				// no size limit
				while ( cin ) {
					cin >> c;
					buf += c;
				}
			}
			
			// parse stdin
			this->parse_multipart( content_type, buf );
		}
	}
}

/// 取得CGI参数
/// \param name CGI参数名,大小写敏感
/// \return 成功返回CGI参数值,否则返回空字符串,多个同名CGI参数值之间分隔符为半角空格' '
string Cgi::get_cgi( const string &name ) {
	if ( name == "" ) 
		return string( "" );
	
	if ( _method=="GET" || _method=="POST" ) {
		if ( _cgi.find(name) != _cgi.end() )
			return _cgi[name];
		else 
			return string( "" );
	}
	
	else if ( _method != "OPTIONS" && _method != "HEAD" && _method != "PUT" &&
			  _method != "DELETE" && _method != "TRACE" ) {
		// 终端测试模式，用户输入 cgi 参数
		string cgival;
		cout << "Input value of CGI parameter \"" << name << "\", type _SPACE_ if no value: ";
		cin >> cgival;
		if ( cgival != "_SPACE_" )
			return cgival;
		else
			return string( "" );
	}
	
	else 
		return string( "" );
}

/// 保存CGI参数
/// \param name CGI参数名,大小写敏感
/// \param value CGI参数值
void Cgi::add_cgi( const string &name, const string &value ) {
	if ( _cgi[name] == "" )
		_cgi[name] = value;
	else
		_cgi[name] += ( " " + value );
}

/// 分析urlencoded类型内容
/// \param buf 要分析的内容
void Cgi::parse_urlencoded( const string &buf ) {
	/*****************************
	name1=value1&name2=value2&...
	*****************************/

	String buffer = buf;
	vector<String> pairslist = buffer.split( "&" );
	
	for ( size_t i=0; i<pairslist.size(); ++i ) {
		String name = pairslist[i].substr( 0, pairslist[i].find("=") );
		String value = pairslist[i].substr( pairslist[i].find("=")+1 );

		name.replace_all( "+", " " );
		name = uri_decode( name );
		value.replace_all( "+", " " );
		value = uri_decode( value );
		
		add_cgi( name, value );
	}
}

/// 分析multipart类型内容,
/// HTML FORM 参数为 enctype=multipart/form-data
/// \param content_type Content-Type描述字符串
/// \param buf 要分析的内容
void Cgi::parse_multipart( const string &content_type, const string &buf ) {
	/*******************************************************
	设分隔符为{boundary}，回车(0x0D)和换行符(0x0A)为<CR>
	
	multipart/form-data, boundary={boundary}
	
	1.文件型参数的multipart格式：
	--{boundary}<CR>
	Content-Disposition: form-data; name="参数名称"; filename="文件名称"<CR>
	Content-Type: {Content-Type}<CR>
	<CR>
	文件内容
	<CR>
	--{boundary}<CR>
	
	2.普通参数的multipart格式：
	--{boundary}<CR>
	Content-Disposition: form-data; name="参数名称"<CR>
	<CR>
	参数值
	<CR>
	--{boundary}<CR>
	*******************************************************/
	
	String buffer = buf;
	size_t pos = 0;
	const char cr[3] = "\x0D\x0A";
	String boundary = "";
	
	// get boundary
	/*****************************************************
	multipart/form-data, boundary={boundary}
	*****************************************************/
	
	if ( (pos=content_type.find("boundary=")) != content_type.npos )
		boundary = ( "--" + content_type.substr(pos+9) ); // 9: strlen("boundary=")
	else return; // format error
	
	// split by boundary
	vector<String> item = buffer.split( boundary );
	for ( size_t i=0; i<item.size(); ++i ) {
		if ( item[i].length() > 0 ) {
			String itembuf = item[i];
			size_t nextpos = 0;
			String itemname;
			String itemvalue;

			// split tags
			String crcr = cr; crcr += cr;	// <CR><CR>
			String qucr = "\""; qucr += cr;	// "<CR>
			String qucrcr = qucr + cr;		// "<CR><CR>

			if ( (pos=itembuf.find("; name=\"")) != itembuf.npos ) {
				size_t filename_pos = 0;
				if ( (filename_pos=itembuf.find("; filename=\"")) != itembuf.npos ) {					
					// 文件型参数
					/******************************************************
					Content-Disposition: form-data; name="参数名称"; filename="文件名称"<CR>
					Content-Type: {Content-Type}<CR>
					<CR>
					文件内容
					<CR>
					******************************************************/
					
					// 分析结果
					/******************************************************
					参数名称 = 文件内容
					参数名称_name = 文件名称
					参数名称_type = {Content-Type}
					******************************************************/

					String filename;
					String filetype;
					
					// 参数名称
					if ( filename_pos > (pos+9) ) // 9: strlen("; name=\"\"")
						itemname = itembuf.substr( pos+8, filename_pos-pos-9 ); // 9->8?

					if ( itemname != "" ) {
						// 参数名称_name = 文件名称
						if ( (nextpos=itembuf.find(qucr,filename_pos)) != itembuf.npos
							&& nextpos >= (filename_pos+12) ) { // 12: strlen("; filename=\"")						
							filename = itembuf.substr( filename_pos+12, nextpos-filename_pos-12 );
							if ( filename != "" )
								add_cgi( (itemname+"_name"), filename );
						}

						// 参数名称_type = {Content-Type}
						if ( (pos=itembuf.find("Content-Type: ",filename_pos)) != itembuf.npos
							&& (nextpos=itembuf.find(crcr,pos)) != itembuf.npos
							&& nextpos >= (pos+14) ) { // 14: strlen("Content-Type: ")
							
							filetype = itembuf.substr( pos+14, nextpos-pos-14 );
							if ( filetype != "" )
								add_cgi( (itemname+"_type"), filetype );
						}

						// 参数名称 = 文件内容
						if ( (pos=nextpos+4) != itembuf.npos // 4: strlen("")
							&& (nextpos=itembuf.rfind(cr)) != itembuf.npos
							&& nextpos >= (pos+1) ) {
							itemvalue = itembuf.substr( pos, nextpos-pos );
							if ( itemvalue!= "" )
								add_cgi( itemname, itemvalue );
						}
					}
				} // 文件型参数
				
				else {
					// 普通参数
					/******************************************************
					Content-Disposition: form-data; name="参数名称"<CR>
					<CR>
					参数值
					<CR>
					******************************************************/
					
					// 参数名称
					if ( (nextpos=itembuf.find(qucr,pos)) != itembuf.npos )
						itemname = itembuf.substr( pos+8, nextpos-pos-8 ); // 8: strlen("; name=\"")
					
					// 参数名称 = 参数值
					if ( itemname != ""
						&& (pos=itembuf.find(qucrcr)) != itembuf.npos 
						&& (nextpos=itembuf.rfind(cr)) != itembuf.npos
						&& nextpos >= (pos+6) ) { // 6: strlen("\"<CR>\n")
						// 5: strlen("\"<CR>") 6: strlen("\"<CR>\n")
						itemvalue = itembuf.substr( pos+5, nextpos-pos-5 );
						if ( itemvalue != "" )
							add_cgi( itemname, itemvalue );
					}
				}// 普通参数
			}
		} // one item
	} // split by boundary
	
}

////////////////////////////////////////////////////////////////////////////
// Cookie

/// 构造函数
/// 读取并分析Cookie环境变量
Cookie::Cookie() {
	this->parse_cookie( get_env("HTTP_COOKIE") );
}

/// 取得cookie内容
/// \param name cookie参数名,大小写敏感
/// \return 成功返回cookie参数值,否则返回空字符串
string Cookie::get_cookie( const string &name ) {
	if ( name!="" && _cookies.find(name)!=_cookies.end() )
		return _cookies[name];
	else
		return string( "" );
}

/// 设置cookie内容
/// 必须在输出content-type前调用
/// \param name cookie名字
/// \param value cookie值
/// \param expires cookie有效期,GMT格式日期字符串,默认为空
/// \param path cookie路径,默认为"/"
/// \param domain cookie域,默认为""
void Cookie::set_cookie( const string &name, const string &value, 
	const string &expires, const string &path, const string &domain ) const 
{
	// Set-Cookie: name=value; expires=expires; path=path; domain=domain;
	
	string expires_setting;
	if ( expires != "" )
		expires_setting = "expires=" + expires + "; ";
	else
		expires_setting = "";
	
	cout << "Set-Cookie: " + name + "=" + value + "; "
		 << expires_setting
		 << "path=" + path + "; "
		 << "domain=" + domain + ";" << endl;
}

/// 分析cookie内容
/// \param buf 要分析的内容
void Cookie::parse_cookie( const string &buf ) {
	/*****************************
	name1=value1; name2=value2; ...
	*****************************/

	String buffer = buf;
	vector<String> pairslist = buffer.split( "; " );
	
	for ( size_t i=0; i<pairslist.size(); ++i ) {
		String name = pairslist[i].substr( 0, pairslist[i].find("=") );
		String value = pairslist[i].substr( pairslist[i].find("=")+1 );

		name.replace_all( "+", " " );
		name = uri_decode( name );
		value.replace_all( "+", " " );
		value = uri_decode( value );
		
		_cookies[name] = value;
	}
}

} // namespace
