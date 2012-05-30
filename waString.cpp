/// \file waString.cpp
/// webapp::String类实现文件

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <set>
#include <fstream>
#include "waString.h"

/// Web Application Library namaspace
namespace webapp {
	
////////////////////////////////////////////////////////////////////////////////	

/// \defgroup waString waString相关全局函数

/// \ingroup waString
/// \fn string itos( const long i, const ios::fmtflags base )
/// long int转换为string
/// \param i long int或者int
/// \param base 转换进制参数,可选
/// - ios::dec 10进制
/// - ios::oct 8进制
/// - ios::hex 16进制
/// - 默认为10进制
/// \return 返回结果string,转换失败返回"0"
string itos( const long i, const ios::fmtflags base ) {
	char format[] = "%ld";
	if ( base == ios::oct )
		strcpy( format, "%o" );
	else if ( base == ios::hex )
		strcpy( format, "%X" );

	// try
	int strlen = 32;
	char *buf = new char[strlen];
	memset( buf, 0, strlen );
	int size = snprintf( buf, strlen, format, i );
	if ( size >= strlen ) {
		// retry
		delete[] buf;
		buf = new char[size+1];
		memset( buf, 0, size+1 );
		snprintf( buf, size+1, format, i );
	}
	
	string result( buf );
	delete[] buf;
	return result;
}

/// \ingroup waString
/// \fn long stoi( const string &s, const ios::fmtflags base )
/// string转换为long int
/// \param s string
/// \param base 转换进制参数,可选
/// - ios::dec 10进制
/// - ios::oct 8进制
/// - ios::hex 16进制
/// - 默认为10进制
/// \return 返回结果long int,转换失败返回0
long stoi( const string &s, const ios::fmtflags base ) {
	int ibase = 10;
	char *ep;
	
	if ( base == ios::hex )
		ibase = 16;
	else if ( base == ios::oct )
		ibase = 8;
		
	String ps = s;
	ps.trim();
	return strtol( ps.c_str(), &ep, ibase );
}

/// \ingroup waString
/// \fn string ftos( const doule f, const int ndigit )
/// double转换为string
/// \param f double
/// \param ndigit 小数点后保留位数,默认为2
/// \return 转换成功返回string,否则返回"0"
string ftos( const double f, const int ndigit ) {
	int fmtlen = 10;
	int strlen = 64;
	int buflen;
	
	// create format string
	char *fmt = new char[fmtlen];
	memset( fmt, 0, fmtlen );
	buflen = snprintf( fmt, fmtlen, "%%.%df", ndigit );
	if ( buflen >= fmtlen ) {
		delete[] fmt;
		fmt = new char[buflen+1];
		memset( fmt, 0, buflen+1 );
		snprintf( fmt, buflen+1, "%%.%df", ndigit );
	}
	
	// convert
	char *str = new char[strlen];
	memset( str, 0, strlen );
	buflen = snprintf( str, strlen, fmt, f );
	if ( buflen >= strlen ) {
		delete[] str;
		str = new char[buflen+1];
		memset( str, 0, buflen+1 );
		snprintf( str, buflen+1, fmt, f );
	}

	string s = str;
	delete[] fmt;
	delete[] str;
	return s;
}

/// \ingroup waString
/// \fn double stof( const string &s )
/// string转换为double
/// \param s string
/// \return 转换成功返回double,否则返回0
double stof( const string &s ) {
	char *ep;
	return strtod( s.c_str(), &ep );
}

/// \ingroup waString
/// \fn bool isgbk( const unsigned char c1, const unsigned char c2 )
/// 判断一个双字节字符是否是GBK编码汉字
/// \param c1 双字节字符1
/// \param c2 双字节字符2
/// \retval true 是
/// \retval false 否
bool isgbk( const unsigned char c1, const unsigned char c2 ) {
	if ( (c1>=0x81&&c1<=0xFE) && ((c2>=0x40&&c2<=0x7E)||(c2>=0xA1&&c2<=0xFE)) )
		return true;
	else
		return false;
}

/// \ingroup waString
/// \fn string va_sprintf( va_list ap, const string &format )
/// 可变参数字符串格式化，与va_start()、va_end()宏配合使用
/// \param format 字符串格式
/// \param ap 可变参数列表
/// \return 格式化字符串结果
string va_sprintf( va_list ap, const string &format ) {
	int strlen = 256;
	char *buf = new char[strlen];
	memset( buf, 0, strlen );

	int size = vsnprintf( buf, strlen, format.c_str(), ap );
	if ( size >= strlen ) {
		delete[] buf; buf = NULL;
		buf = new char[size+1];
		memset( buf, 0, size+1 );
		vsnprintf( buf, size+1, format.c_str(), ap );
	}
	
	string result = buf;
	delete[] buf; buf = NULL;
	return result;
}

/// \ingroup waString
/// \fn string va_str( const char *format, ... )
/// 格式化字符串并返回，各参数定义与标准sprintf()函数完全相同
/// \return 格式化字符串结果
string va_str( const char *format, ... ) {
	va_list ap;
	va_start( ap, format );
	string result = va_sprintf( ap, format );
	va_end( ap );
	return result;
}

/// 返回 char* 型结果，调用者必须调用 delete[] 释放所返回内存
/// \return char*类型数据结果
char* String::c_char() const {
	size_t size = this->length();
	char *buf = new char[ size + 1 ];
	memset( buf, 0, size );
	strncpy( buf, this->c_str(), size );
	return buf;
}

/// 返回字符数量，GBK汉字算作一个字符
/// \return 字符数量
string::size_type String::w_length() const {
	size_t wlen = 0;
	size_t len = this->length();
	
	for ( size_t i=0; i<len; ++i )  {
		if ( i<(len-1) && isgbk(this->at(i),this->at(i+1)) ) 
			++i;
		++wlen;
	}
	
	return wlen;
}

/// 截取子字符串,避免出现半个汉字
/// 若截取结果的首尾为半个汉字则删除,删除半个汉字后结果可能为空字符串,
/// 该函数避免在截取时将一个完整汉字分开,对字符串中原有的不完整汉字字符不作处理
/// \param pos 起始位置,默认为0,单字节计数方式
/// \param n 要截取的字符串长度,默认为到末尾,单字节计数方式
/// \return 所截取的字符串
String String::w_substr( const string::size_type pos, 
	const string::size_type n ) const 
{
    size_t len = this->length();
    if ( len<=0 || pos>=len || n<=0 )
        return String( "" );

    size_t from = pos;
    size_t to = min( pos+n, len );

    // location
    for ( size_t i=0; i<to; ++i ) {
        if ( (i+1)<len && isgbk(this->at(i),this->at(i+1)) ) {
            if ( i == from-1 )
                ++from;
            else if ( i == to-1 )
                --to;
            ++i;
        }
    }

    // substr
    if ( to > from )
        return String( this->substr(from,to-from) );
    else
        return String( "" );
}

/// 清除左侧空白字符
/// \param blank 要过滤掉的空白字符列表,默认为webapp::BLANK_CHARS
void String::trim_left( const string &blank ) {
	while ( this->length()>0 && blank.find(this->at(0))!=npos )
		this->erase( 0, 1 );
}

/// 清除右侧空白字符
/// \param blank 要过滤掉的空白字符列表,默认为webapp::BLANK_CHARS
void String::trim_right( const string &blank ) {
	while ( this->length()>0 && blank.find(this->at(length()-1))!=npos )
		erase( this->length()-1, 1 );
}

/// 清除两侧空白字符
/// \param blank 要过滤掉的空白字符列表,默认为webapp::BLANK_CHARS
void String::trim( const string &blank ) {
	this->trim_left( blank );
	this->trim_right( blank );
}

/// 从左边截取指定长度子串
/// \param n 要截取的字符串长度,若长度超出则返回原字符串
/// \return 所截取的字符串
String String::left( const string::size_type n ) const {
	size_t len = this->length();
	len = ( n>len ) ? len : n;
	return String( this->substr(0,len) );
}

/// 从中间截取指定长度子串
/// \param pos 开始截取的位置
/// \param n 要截取的字符串长度,若长度超出则返回原字符串,默认为到末尾
/// \return 所截取的字符串
String String::mid( const string::size_type pos, 
	const string::size_type n ) const 
{
	if ( pos > this->length() )	
		return String( "" );
	return String( this->substr(pos,n) );
}		

/// 从右边截取指定长度子串
/// \param n 要截取的字符串长度,若长度超出则返回原字符串
/// \return 所截取的字符串
String String::right( const string::size_type n ) const {
	size_t len = this->length();
	len = ( n>len )? n : len;
	return String( this->substr(len-n,n) );
}		

/// 调整字符串长度
/// \param n 新字符串长度,若小于当前长度则截断,若大于当前长度则补充空白字符
void String::resize( const string::size_type n ) {
	size_t len = this->length();
	if ( n < len ) {
		*this = this->substr( 0, n );
	} else if ( n > len ) {
		for ( size_t i=0; i<(n-len); ++i )
			this->append( " " );
	}
}

/// 统计指定子串出现的次数
/// \param str 要查找的子串
/// \return 子串不重复出现的次数
int String::count( const string &str ) const {
	size_t pos = 0;
	size_t count = 0;
	size_t step = str.length();
	
	while( (pos=this->find(str,pos)) != npos ) {
		++count;
		pos += step;
	}
	
	return count;
}
	
/// 根据分割符分割字符串
/// \param tag 分割标记字符串
/// \param limit 分割次数限制,默认为0即不限制
/// \param mode 结果返回模式,可选
/// - String::SPLIT_IGNORE_BLANK 忽略连续多个分隔符，返回结果不含空字段
/// - String::SPLIT_KEEP_BLANK 不忽略连续多个分隔符，返回结果包含空字段
/// - 默认为String::SPLIT_IGNORE_BLANK
/// \return 分割结果字符串数组 vector<String>
vector<String> String::split( const string &tag, const int limit, 
	const split_mode mode ) const 
{
	string src = *this;
	string curelm;
	vector<String> list;
	int count = 0;

	list.clear();
	if ( tag.length()>0 && src.length()>0 ) {
		// how to split
		size_t pos = src.find( tag );
		
		while ( pos < src.length() ) {
			curelm = src.substr( 0, pos );
			
			// is keep blank
			if ( !(mode==SPLIT_IGNORE_BLANK && curelm.length()==0) ) {
				list.push_back( curelm );
				++count;
			}

			// split
			src = src.substr( pos + tag.length() );
			pos = src.find( tag );
			
			if ( limit>0 && count>=limit )
				break;
		}
		
		// is keep blank
		if ( !(mode==SPLIT_IGNORE_BLANK && src.length()==0) )
			list.push_back( src );
	}
	
	return list;
}

/// 转换字符串为MAP结构(map<string,string>)
/// \param itemtag 表达式之间的分隔符,默认为"&"
/// \param exptag 表达式中变量名与变量值之间的分隔符,默认为"="
/// \return 转换结果 map<string,string>
map<string,string> String::tomap( const string &itemtag, 
	const string &exptag ) const 
{
	map<string,string> hashmap;
	
	if ( itemtag!="" && exptag!="" ) {
		vector<String> items = this->split( itemtag );
		string name, value;
		size_t pos;
		for ( size_t i=0; i< items.size(); ++i ) {           
			pos = items[i].find( exptag );
			name = (items[i]).substr( 0, pos );
			value = (items[i]).substr( pos+exptag.length() );
			if ( name != "" )
				hashmap.insert( map<string,string>::value_type(name,value) );
		}
	}
	
	return hashmap;
}

/// 组合字符串,与split()相反
/// \param strings 字符串数组
/// \param tag 组合分隔符
void String::join( const vector<string> &strings, const string &tag ) {
	if ( strings.size() > 0 ) {
		this->erase();
		*this = strings[0];
		
		for ( size_t i=1; i<strings.size(); ++i )
			*this += ( tag + strings[i] );
	}
}
// for vector<String>
void String::join( const vector<String> &strings, const string &tag ) {
	if ( strings.size() > 0 ) {
		this->erase();
		*this = strings[0];
		
		for ( size_t i=1; i<strings.size(); ++i )
			*this += ( tag + strings[i] );
	}
}

/// 格式化赋值
/// 各参数定义与标准sprintf()函数完全相同
/// \retval true 执行成功
/// \retval false 失败
bool String::sprintf( const char *format, ... ) {
	va_list ap;
	va_start( ap, format );
	*this = va_sprintf( ap, format );
	va_end( ap );
	return true;
}

/// 替换
/// 该函数重载了string::replace()
/// \param oldstr 被替换掉的字符串
/// \param newstr 用来替换旧字符串的新字符串
/// \retval 1 替换成功
/// \retval 0 失败
int String::replace( const string &oldstr, const string &newstr ) {
	size_t pos = 0;
	if ( oldstr!="" && (pos=this->find(oldstr))!=npos ) {
		string::replace( pos, oldstr.length(), newstr );
		return 1;
	}
	return 0;
}

/// 全文替换
/// \param oldstr 被替换掉的字符串
/// \param newstr 用来替换旧字符串的新字符串
/// \return 执行替换的次数
int String::replace_all( const string &oldstr, const string &newstr ) {
	if ( oldstr == "" )
		return 0;
	
	int i = 0;
	size_t pos = 0;
	size_t curpos = 0;
	while ( (pos=this->find(oldstr,curpos)) != npos ) {
		string::replace( pos, oldstr.length(), newstr );
		curpos = pos + newstr.length();
		++i;
	}
	return i;
}

/// 转换为大写字母
void String::upper() {
	for( size_t i=0; i<this->length(); i++ )
		(*this)[i] = toupper( (*this)[i] );
}

/// 转换为小写字母
void String::lower() {
	for( size_t i=0; i<this->length(); i++ )
		(*this)[i] = tolower( (*this)[i] );
}

/// 字符串是否完全由数字组成
/// \retval true 是
/// \retval false 否
bool String::isnum() const {
	if ( this->length() == 0 )
		return false;
		
	for ( size_t i=0; i<this->length(); ++i ) {
		if ( !isdigit((*this)[i]) )
			return false;
	}
	return true;
}

/// 读取文件到字符串
/// \param filename 要读取的文件完整路径名称
/// \retval true 读取成功
/// \retval false 失败
bool String::load_file( const string &filename ) {
	FILE *fp = fopen( filename.c_str(), "rb" );
	if ( fp == NULL ) return false;
	
	// read file size
	fseek( fp, 0, SEEK_END );
	int bufsize = ftell( fp );
	rewind( fp );
	
	char *buf = new char[bufsize+1];
	memset( buf, 0, bufsize+1 );
	fread( buf, 1, bufsize, fp );
	fclose( fp );
	*this = buf;
	delete[] buf;
	return true;
}

/// 保存字符串到文件
/// \param filename 要写入的文件路径名称
/// \param mode 写入方式,默认为ios::trunc|ios::out
/// \param permission 文件属性参数，默认为0666
/// \retval true 写入成功
/// \retval false 失败
bool String::save_file( const string &filename, const ios::openmode mode,
	const mode_t permission ) const 
{
	ofstream outfile( filename.c_str(), mode );
	if ( outfile ) {
		outfile << *this;
		outfile.close();
		
		// chmod
		mode_t mask = umask( 0 );
		chmod( filename.c_str(), permission );
		umask( mask );
		return true;
	}
	return false;
}

} // namespace

