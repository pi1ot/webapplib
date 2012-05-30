/// \file waUtility.cpp
/// 系统调用工具函数实现文件

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdarg>

// for host_addr()
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "waString.h"
#include "waDateTime.h"
#include "waUtility.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {
	
/// \ingroup waUtility 
/// \fn unsigned int string_hash( const string &str )
/// 返回字符串HASH值，基于DJB HASH算法
/// Perl兼容实现版本 string_hash.pl
/// JavaScript兼容实现版本 string_hash.js
/// \param str 源字符串
/// \return 字符串HASH结果，无符号整数
unsigned int string_hash( const string &str ) {
	unsigned char ch;
	unsigned int hash = 5381;
	int len = str.length();
	char *p = const_cast<char*>( str.c_str() );
	
	while ( len > 0 ) {
		ch = *p++ - 'A';
		if ( ch <= ('Z'-'A') )
			ch += ('a'-'A');
		hash = ( (hash<<5) + hash ) ^ ch;
		--len;
	}
	return hash;
}	

// 判断是否标点符号字符
bool is_punctuation( unsigned char c ) {
	const char filter_char[] = "`[]~@^_{}|\\";
	if ( strchr(filter_char,c) != NULL )
		return true;
	else
		return false;
}

// 全角字母、数字、标点、空白字符转换为半角字符
string sbc_to_dbc( const string &sbc_string ) {
	string dbc;
	if ( sbc_string == "" ) return dbc;
	
	char sbc_chr[3];
	unsigned int len = sbc_string.length();

	dbc.reserve( len );
	for ( unsigned int i=0; i<len; ++i ) {
		if ( i<(len-1) && isgbk(sbc_string[i],sbc_string[i+1]) ) {
			// double byte char
			sbc_chr[0] = sbc_string[i];
			sbc_chr[1] = sbc_string[++i];
			sbc_chr[2] = '\0';
			
			bool chinese_char = true;
			for ( int j=0; j<SDBC_TABLE_SIZE; ++j ) {
				if ( strncmp(sbc_chr,SBC_TABLE[j],3) == 0 ) {
					// alpha, digit, punct, space
					dbc += DBC_TABLE[j];
					chinese_char = false;
					break;
				}
			}
			if ( chinese_char )
				dbc += sbc_chr;
		} else {
			// single byte char
			dbc += sbc_string[i];
		}
	}
	
	return dbc;
}

/// \ingroup waUtility 
/// \fn string extract_html( const string &html )
/// 提取HTML代码正文
/// \param html HTML代码字符串
/// \return 不含HTML代码的提取结果
string extract_html( const string &html ) {
	bool in_html = false;
	string text, curr_tag;
	text.reserve( html.length() );
	
	for ( unsigned int i=0; i<html.length(); ++i ) {
		if ( !in_html && html[i]=='<' ) {
			// <...
			in_html = true;
			curr_tag = "<";
		} else if ( in_html && html[i]=='>' ) {
			// >...
			in_html = false;
		} else if ( !in_html ) {
			// ...
			text += html[i];
		} else if ( in_html ) {
			// <...>
			curr_tag += html[i];
		}
	}

	if ( in_html ) // unclosed html code
		text += curr_tag;
	return text;
}

/// \ingroup waUtility 
/// \fn string extract_text( const string &text, const int option, const size_t len )
/// 全角半角字符转换并提取正文
/// \param text 源字符串
/// \param option 过滤范围选项，可选值组合有
/// - EXTRACT_ALPHA 过滤字母
/// - EXTRACT_DIGIT 过滤数字
/// - EXTRACT_PUNCT 过滤标点
/// - EXTRACT_SPACE 过滤空白
/// - EXTRACT_HTML 过滤HTML代码
/// - 默认值为EXTRACT_ALL即以上全部
/// \param len 过滤长度，大于0时只截取前len个有效字符，默认为0
/// \return 转换提取结果字符串，若源字符串内容被全部过滤则返回空
string extract_text( const string &text, const int option, const size_t len ) {
	if ( text=="" || option<=0 )
		return text;
	
	string converted = sbc_to_dbc( text );
	
	// is HTML
	if ( option&EXTRACT_HTML )
		converted = extract_html( converted );
	if ( option == EXTRACT_HTML )
		return converted;

	string extracted;
	extracted.reserve( text.length() );
	
	for ( unsigned int i=0; i<converted.length(); ++i ) {
		unsigned char c = converted[i];
		if ( isalpha(c) )
			c = tolower( c );
		
		// is GBK char
		if ( !is_punctuation(c) && !isalpha(c) && 
			 ((c>=0x81&&c<=0xFE) || (c>=0x40&&c<=0x7E) || (c>=0xA1&&c<=0xFE)) )
			extracted += c;
		// is alpha
		else if ( option&EXTRACT_ALPHA && isalpha(c) )
			continue;
		// is digit
		else if ( option&EXTRACT_DIGIT && isdigit(c) )
			continue;
		// is punct
		else if ( option&EXTRACT_PUNCT && (ispunct(c)||is_punctuation(c)) )
			continue;
		// is space
		else if ( option&EXTRACT_SPACE && (isspace(c)||isblank(c)) )
			continue;
		// other 
		else
			extracted += c;
		
		// enough
		if ( len>0 && extracted.length()>=len )
			break;
	}
	
	return extracted;
}

// 底层日志函数实现
void _file_logger( FILE *fp, va_list ap, const char *format ) {
	if ( fp == NULL ) return;
	
	// prefix datetime
	DateTime now;
	String logformat;
	logformat.sprintf( "%s\t%s", now.datetime().c_str(), format );

	// log content
	vfprintf( fp, logformat.c_str(), ap );
	fputc( '\n', fp );
}

/// \ingroup waUtility 
/// \fn void file_logger( const string &file, const char *format, ... )
/// 追加日志记录
/// \param file 日志文件路径
/// \param format 日志行格式
/// \param ... 日志数据参数列表
void file_logger( const string &file, const char *format, ... ) {
	if ( file == "" ) return;
	
	FILE *fp = fopen( file.c_str(), "a" );
	if ( fp != NULL ) {
		va_list ap;
		va_start( ap, format );
		_file_logger( fp, ap, format );
		va_end( ap );
		fclose( fp );
	}
}

/// \ingroup waUtility 
/// \fn void file_logger( FILE *fp, const char *format, ... )
/// 追加日志记录
/// \param fp 日志文件句柄，或者stdout/stderr
/// \param format 日志行格式
/// \param ... 日志数据参数列表
void file_logger( FILE *fp, const char *format, ... ) {
	if ( fp == NULL ) return;
	
	va_list ap;
	va_start( ap, format );
	_file_logger( fp, ap, format );
	va_end( ap );
}

/// \ingroup waUtility 
/// \fn string system_command( const string &cmd )
/// 执行命令并返回命令输出结果
/// \param cmd 命令字符串，包括命令行参数
/// \return 命令执行输出结果
string system_command( const string &cmd ) {
	string res;
	if ( cmd == "" ) return res;
	
	char buf[256] = {0};
	FILE *pp = popen( cmd.c_str(), "r" );
	if( pp==NULL || pp==(void*)-1 )
		return res;
	while ( fgets(buf,sizeof(buf),pp) )
		res += buf;
		
	pclose( pp );
	return res;
}

/// \ingroup waUtility 
/// \fn string host_addr( const string &interface )
/// 返回指定网卡设备绑定的IP地址
/// \param interface 网卡设备名，默认为"eth0"
/// \return 指定网卡设备绑定的IP地址
string host_addr( const string &interface ) {
	int fd;
	if ( (fd=socket(AF_INET,SOCK_DGRAM,0)) < 0 )
		return string("");
	
	ifreq ifr;
	memset( &ifr, 0, sizeof(ifr) );
	strncpy( ifr.ifr_name, interface.c_str(), sizeof(ifr.ifr_name)-1 );

	sockaddr_in *sin;
	sin = (struct sockaddr_in *)&ifr.ifr_addr;
	sin->sin_family = AF_INET;
	
	if ( ioctl(fd,SIOCGIFADDR,&ifr) < 0 ) {
		close( fd );
		return string("");
	}
	
	static char buf[256] = {0};
	if( inet_ntop(AF_INET,(void *)&sin->sin_addr,buf,sizeof(buf)-1) < 0 ) {
		close( fd );
		return string("");
	}
	
	close( fd );
	return string(buf);
}

} // namespace

