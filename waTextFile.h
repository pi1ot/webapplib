/// \file waTextFile.h
/// 固定分隔符文本文件读取解析类头文件
/// 读取解析固定分隔符文本文件
/// 依赖于 webapp::String

#ifndef _WEBAPPLIB_TEXTFILE_H_
#define _WEBAPPLIB_TEXTFILE_H_ 

#include <cstdio>
#include "waString.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {
	
/// 固定分隔符文本文件读取解析类
class TextFile {
	public:
		
	/// 默认构造函数
	TextFile():_fp(0), _line(0), _len(0)
	{};
	
	/// 参数为文本文件名的构造函数
	TextFile( const string &file )
	:_fp(0), _line(0), _len(0) {
		this->open( file );
	}
	
	/// 析构函数
	~TextFile() {
		this->close();
	}
	
	/// 打开文本文件
	bool open( const string &file );
	
	/// 关闭文本文件
	void close();
	
	/// 读取下一行
	bool next_line( string &line );
	
	/// 读取下一行并按分隔符拆分字段
	bool next_fields( vector<String> &fields, const string &split="\t", const int limit=0 );
	
	////////////////////////////////////////////////////////////////////////////
	private:
		
	FILE *_fp;
	char *_line;
	size_t _len;
};
	
} // namespace

#endif //_WEBAPPLIB_TEXTFILE_H_

