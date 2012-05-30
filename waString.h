/// \file waString.h
/// webapp::String类头文件
/// 继承自string的字符串类
/// <a href=std_string.html>基类string使用说明文档</a>

#ifndef _WEBAPPLIB_STRING_H_
#define _WEBAPPLIB_STRING_H_ 

#include <sys/stat.h>
#include <cstdarg>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

/// Web Application Library namaspace
namespace webapp {
	
////////////////////////////////////////////////////////////////////////////////	
// 空白字符列表
const char BLANK_CHARS[] = " \t\n\r\v\f";

////////////////////////////////////////////////////////////////////////////////
/// long int转换为string
string itos( const long i, const ios::fmtflags base = ios::dec );
/// string转换为int
long stoi( const string &s, const ios::fmtflags base = ios::dec );

/// double转换为string
string ftos( const double f, const int ndigit = 2 );
/// string转换为double
double stof( const string &s );

/// 判断一个双字节字符是否是GBK编码汉字
bool isgbk( const unsigned char c1, const unsigned char c2 );

/// 可变参数字符串格式化，与va_start()、va_end()宏配合使用
string va_sprintf( va_list ap, const string &format );
/// 格式化字符串并返回
string va_str( const char *format, ... );

////////////////////////////////////////////////////////////////////////////////
/// 继承自string的字符串类
/// <a href="std_string.html">基类string使用说明文档</a>
class String : public string {
	public:
	
	////////////////////////////////////////////////////////////////////////////
	/// 默认构造函数
	String(){}
	
	/// 参数为char*的构造函数
	String( const char *s ) {
		if( s ) this->assign( s );
		else this->erase();
	}
	
	/// 参数为string的构造函数
	String( const string &s ) {
		this->assign( s );
	}
	
	/// 析构函数
	virtual ~String(){}
	
	////////////////////////////////////////////////////////////////////////////
	/// \enum 函数String::split()分割结果返回方式
	enum split_mode {
		/// 忽略连续多个分隔符，返回结果不含空字段
		SPLIT_IGNORE_BLANK,
		/// 不忽略连续多个分隔符，返回结果包含空字段
		SPLIT_KEEP_BLANK
	};	

	////////////////////////////////////////////////////////////////////////////
	/// 返回 char* 型结果，调用者必须调用 delete[] 释放所返回内存
	char* c_char() const;
	
	/// 返回字符数量，支持全角字符
	string::size_type w_length() const;
	/// 截取子字符串，支持全角字符
	String w_substr( const string::size_type pos = 0, 
		const string::size_type n = npos ) const;

	/// 清除左侧空白字符
	void trim_left( const string &blank = BLANK_CHARS );
	/// 清除右侧空白字符
	void trim_right( const string &blank = BLANK_CHARS );
	/// 清除两侧空白字符
	void trim( const string &blank = BLANK_CHARS );

	/// 从左边截取指定长度子串
	String left( const string::size_type n ) const;
	/// 从中间截取指定长度子串
	String mid( const string::size_type pos, 
		const string::size_type n = npos ) const;
	/// 从右边截取指定长度子串
	String right( const string::size_type n ) const;
	
	/// 调整字符串长度
	void resize( const string::size_type n );

	/// 统计指定子串出现的次数
	int count( const string &str ) const;
	
	/// 根据分割符分割字符串
	vector<String> split( const string &tag, const int limit = 0, 
		const split_mode mode = SPLIT_IGNORE_BLANK ) const;
	
	/// 转换字符串为MAP结构(map<string,string>)
	map<string,string> tomap( const string &itemtag = "&", 
		const string &exptag = "=" ) const;

	/// 组合字符串
	void join( const vector<string> &strings, const string &tag );
	void join( const vector<String> &strings, const string &tag );

	/// 格式化赋值
	bool sprintf( const char *format, ... );
	
	/// 替换
	int replace( const string &oldstr, const string &newstr );
	/// 全文替换
	int replace_all( const string &oldstr, const string &newstr );
	
	/// 转换为大写字母
	void upper();
	/// 转换为小写字母
	void lower();
	
	/// 字符串是否完全由数字组成
	bool isnum() const;
	
	/// 读取文件到字符串
	bool load_file( const string &filename );
	/// 保存字符串到文件
	bool save_file( const string &filename, const ios::openmode mode = ios::trunc|ios::out,
		const mode_t permission = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH ) const;
};

} // namespace

#endif //_WEBAPPLIB_STRING_H_

