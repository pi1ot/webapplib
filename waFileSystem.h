/// \file waFileSystem.h
/// 文件操作函数头文件
/// 常用文件操作

#ifndef _WEBAPPLIB_FILE_H_
#define _WEBAPPLIB_FILE_H_ 

#include <cstdio>
#include <string>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>

using namespace std;

/// Web Application Library namaspace
namespace webapp {

/// 文件或者目录是否存在
bool file_exist( const string &file );

/// 文件是否为链接
bool is_link( const string &file );
/// 是否为目录
bool is_dir( const string &file );

/// 建立链接
bool make_link( const string &srcfile, const string &destfile );

/// 取得文件大小
size_t file_size( const string &file );
/// 取得文件更改时间
time_t file_time( const string &file );
/// 取得文件路径
string file_path( const string &file );
/// 取得文件名称
string file_name( const string &file );

/// 文件或者目录改名
bool rename_file( const string &oldname, const string &newname );
/// 拷贝文件	
bool copy_file( const string &srcfile, const string &destfile );
/// 删除文件
bool delete_file( const string &file );
/// 移动文件
bool move_file( const string &srcfile, const string &destfile );

/// 返回目录文件列表
vector<string> dir_files( const string &dir );
/// 建立目录
bool make_dir( const string &dir, 
	const mode_t mode = S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );
/// 拷贝目录
bool copy_dir( const string &srcdir, const string &destdir );
/// 删除目录
bool delete_dir( const string &dir );
/// 移动目录
bool move_dir( const string &srcdir, const string &destdir );
			  
/// 文件句柄锁函数
void lock_file( int fd, const int type );
/// 文件句柄锁函数
inline void lock_file( FILE *fp, const int type ) {
	lock_file( fileno(fp), type );
}

/// 判断文件句柄锁
bool is_locked( int fd );
/// 判断文件句柄锁
inline bool is_locked( FILE *fp ) {
	return is_locked( fileno(fp) );
}

/// 申请锁并打开文件
FILE* lock_open( const string &file, const char* mode, const int type );

} // namespace

#endif //_WEBAPPLIB_FILE_H_

