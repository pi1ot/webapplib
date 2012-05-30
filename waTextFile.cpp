/// \file waTextFile.cpp
/// 固定分隔符文本文件读取解析类实现文件
/// 读取解析固定分隔符文本文件

#include "waTextFile.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {
	
/// 打开文本文件
/// \param file 文本文件路径名
/// \retval true 打开文件成功
/// \retval false 打开文件失败
bool TextFile::open( const string &file ) {
	if ( file == "" ) return false;
	if ( _fp != NULL ) this->close();
	
	_fp = fopen( file.c_str(), "r" );
	if ( _fp != NULL ) return true;
	else return false;
}

/// 关闭文本文件
void TextFile::close() {
	if ( _fp != NULL ) {
		fclose( _fp );
		_fp = NULL;
	}
	if ( _len>0 && _line!=NULL ) {
		// malloc/realloc by getline()
		free( _line ); 
		_line = NULL;
		_len = 0;
	}
}

/// 读取下一行
/// \param line 读取到的文本行，不含末尾的'\\n'
/// \retval true 还未读到文件末尾
/// \retval false 已读到文件末尾
bool TextFile::next_line( string &line ) {
	if( _fp == NULL ) 
		return false;
	
	int readed = getline( &_line, &_len, _fp );
	if( readed == -1 )
		return false;

	// trim
	if ( _line[readed-1] == '\n' )
		_line[readed-1] = '\0';
	line = _line;

	return true;
}

/// 读取下一行并按分隔符拆分字段
/// \param fields 读取到的字符串数组
/// \param split 字段切分分隔符，默认为'\\t'
/// \param limit 字段切分次数限制，默认为0即不限制
/// \retval true 还未读到文件末尾
/// \retval false 已读到文件末尾
bool TextFile::next_fields( vector<String> &fields, const string &split, const int limit ) {
	String line;
	if ( this->next_line(line) ) {
		fields = line.split( split, limit );
		return true;
	}
	return false;
}
	
} // namespace

