/// \file waFileSystem.cpp
/// 文件操作函数实现文件

#include <cstdio>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/file.h>
#include "waFileSystem.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {

/// \defgroup waFileSystem waFileSystem文件操作函数库
	
/// \ingroup waFileSystem
/// \fn bool file_exist( const string &file )
/// 文件或者目录是否存在
/// \param file 文件路径名
/// \retval true 文件存在
/// \retval false 不存在
bool file_exist( const string &file ) {
	if ( access(file.c_str(),F_OK) == 0 )
		return true;
	else
		return false;
}

/// \ingroup waFileSystem
/// \fn bool is_link( const string &file )
/// 文件是否为链接
/// \param file 文件路径名
/// \retval true 文件存在且为符号链接
/// \retval false 不存在或者不是符号链接
bool is_link( const string &file ) {
	struct stat statbuf;
	
	if( lstat(file.c_str(),&statbuf) == 0 ) {
		if ( S_ISLNK(statbuf.st_mode) != 0 )
			return true;
	}
	return false;
}

/// \ingroup waFileSystem
/// \fn bool is_dir( const string &file )
/// 是否为目录
/// \param dile 目录路径名
/// \retval true 目录存在
/// \retval false 不存在或者不是目录
bool is_dir( const string &file ) {
	struct stat statbuf;
	
	if( stat(file.c_str(),&statbuf) == 0 ) {
		if ( S_ISDIR(statbuf.st_mode) != 0 )
			return true;
	}
	return false;
}

/// \ingroup waFileSystem
/// \fn bool make_link( const string &srcfile, const string &destfile )
/// 建立链接,新链接名文件必须不存在
/// \param srcfile 原文件名
/// \param destfile 新链接名
/// \retval true 操作成功
/// \retval false 不成功
bool make_link( const string &srcfile, const string &destfile ) {
	if ( symlink(srcfile.c_str(),destfile.c_str()) == 0 )
		return true;
	else
		return false;
}

/// \ingroup waFileSystem
/// \fn long file_size( const string &file )
/// 取得文件大小
/// \param file 文件路径名
/// \return 若文件存在则返回大小,否则返回-1
long file_size( const string &file ) {
	struct stat statbuf;
	
	if( stat(file.c_str(),&statbuf)==0 )
		return statbuf.st_size;
	else		
		return -1;
}

/// \ingroup waFileSystem
/// \fn long file_time( const string &file )
/// 取得文件更改时间
/// \param file 文件路径名
/// \return 若文件存在则返回其最后更改时间,否则返回-1
long file_time( const string &file ) {
	struct stat statbuf;
	
	if( stat(file.c_str(),&statbuf)==0 )
		return statbuf.st_mtime;
	else		
		return -1;
}

/// \ingroup waFileSystem
/// \fn string file_path( const string &file )
/// 取得文件路径
/// \param file 文件路径名
/// \return 若能取得文件路径则返回,否则返回空字符串
string file_path( const string &file ) {
	size_t p;
	if ( (p=file.rfind("/")) != file.npos )
		return file.substr( 0, p );
	else if ( (p=file.rfind("\\")) != file.npos )
		return file.substr( 0, p );
	return string( "" );
}

/// \ingroup waFileSystem
/// \fn string file_name( const string &file )
/// 取得文件名称
/// \param file 文件路径名
/// \return 若能取得文件名称则返回,否则返回原文件路径名称
string file_name( const string &file ) {
	size_t p;
	if ( (p=file.rfind("/")) != file.npos )
		return file.substr( p+1 );
	else if ( (p=file.rfind("\\")) != file.npos )
		return file.substr( p+1 );
	return file;
}

/// \ingroup waFileSystem
/// \fn bool rename_file( const string &oldname, const string &newname )
/// 文件或者目录改名,新文件名必须与原文件名位于同一文件系统
/// \param oldname 原文件名
/// \param newname 新文件名
/// \retval true 操作成功
/// \retval false 失败
bool rename_file( const string &oldname, const string &newname ) {
	if ( rename(oldname.c_str(),newname.c_str()) != -1 )
		return true;
	else
		return false;
}

/// \ingroup waFileSystem
/// \fn bool copy_file( const string &srcfile, const string &destfile )
/// 拷贝文件	
/// \param srcfile 原文件名
/// \param destfile 目的文件名,文件属性为0666
/// \retval true 操作成功
/// \retval false 失败
bool copy_file( const string &srcfile, const string &destfile ) {
	FILE *src=NULL, *dest=NULL;
	if ( (src=fopen(srcfile.c_str(),"rb")) == NULL ) {
		return false;
	}
	if ( (dest=fopen(destfile.c_str(),"wb+")) == NULL ) {
		fclose( src );
		return false;
	}
	
	const int bufsize = 8192;
	char buf[bufsize];
	size_t n;
	while ( (n=fread(buf,1,bufsize,src)) >= 1 ) {
		if ( fwrite(buf,1,n,dest) != n ) {
			fclose( src );
			fclose( dest );
			return false;
		}
	}
	
	fclose( src );
	fclose( dest );

	//chmod to 0666
	mode_t mask = umask( 0 );
	chmod( destfile.c_str(), S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH );
	umask( mask );
	
	return true;
}

/// \ingroup waFileSystem
/// \fn bool delete_file( const string &file )
/// 删除文件
/// \param file 文件路径名
/// \retval true 删除成功
/// \retval false 文件不存在或者删除失败
bool delete_file( const string &file ) {
	if ( remove(file.c_str()) == 0 )
		return true;
	else
		return false;
}

/// \ingroup waFileSystem
/// \fn bool move_file( const string &srcfile, const string &destfile )
/// 移动文件
/// \param srcfile 原文件名
/// \param destfile 新文件名
/// \retval true 操作成功
/// \retval false 失败
bool move_file( const string &srcfile, const string &destfile ) {
	if ( rename_file(srcfile,destfile) )
		return true;

	// rename fail, copy and delete file		
	if ( copy_file(srcfile,destfile) ) {
		if ( delete_file(srcfile) )
			return true;
	}
	
	return false;
}

/// \ingroup waFileSystem
/// \fn vector<string> dir_files( const string &dir )
/// 返回目录文件列表
/// \param dir 参数为目录路径名
/// \return 返回结果为文件及子目录列表,子目录的第一个字符为 '/',
/// 返回结果中不包括代表当前及上一级目录的 "/.", "/.."
vector<string> dir_files( const string &dir ) {
	vector<string> files;
	string file;
	DIR *pdir = NULL;
	dirent *pdirent = NULL;
	
	if ( (pdir=opendir(dir.c_str())) != NULL ) {
		while ( (pdirent=readdir(pdir)) != NULL ) {
			file = pdirent->d_name;
			if ( file!="." && file!=".." ) {
				if ( is_dir(dir+"/"+file) ) 
					file = "/"+file;
				files.push_back( file );
			}
		}
		closedir( pdir );
	}
	
	return files;
}

/// \ingroup waFileSystem
/// \fn bool make_dir( const string &dir, const size_t mode )
/// 建立目录
/// \param dir 要创建的目录,若上层目录不存在则自动创建
/// \param mode 创建目录权限,默认为S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH(0755)
/// 成功返回true, 否则返回false.
/// \retval true 操作成功
/// \retval false 失败
bool make_dir( const string &dir, const mode_t mode ) {
	// check
	size_t len = dir.length();
	if ( len <= 0 )	return false;

	string tomake;
	char curchr;
	for( size_t i=0; i<len; ++i ) {
		// append
		curchr = dir[i];
		tomake += curchr;
		if ( curchr=='/' || i==(len-1) ) {
			// need to mkdir
			if ( !file_exist(tomake) && !is_dir(tomake) ) {
				// able to mkdir
				mode_t mask = umask( 0 );				
				if ( mkdir(tomake.c_str(),mode) == -1 ) {
					umask( mask );
					return false;
				}
				umask( mask );
			}
		}
	}
	
	return true;
}	

/// \ingroup waFileSystem
/// \fn bool copy_dir( const string &srcdir, const string &destdir )
/// 拷贝目录,拷贝子目录时为递归调用
/// \param srcdir 原目录
/// \param destdir 目的目录
/// \retval true 操作成功
/// \retval false 失败
bool copy_dir( const string &srcdir, const string &destdir ) {	
	vector<string> files = dir_files( srcdir );
	string from;
	string to;
	
	// 创建目标目录
	if ( !file_exist(destdir) )
		make_dir( destdir );
	
	for ( size_t i=0; i<files.size(); ++i ) {
		from = srcdir + "/" + files[i];
		to = destdir + "/" + files[i];
		
		// 子目录,递归调用
		if ( files[i][0] == '/' ) {
			 if ( !copy_dir(from,to) )
				return false;
		}
		
		// 文件
		else if ( !copy_file(from,to) )
			return false;
	}
	
	return true;
}

/// \ingroup waFileSystem
/// \fn bool delete_dir( const string &dir )
/// 删除目录,删除子目录时为递归调用
/// \param dir 要删除的目录
/// \retval true 操作成功
/// \retval false 失败
bool delete_dir( const string &dir ) {	
	vector<string> files = dir_files( dir );
	string todel;
	
	// 删除文件
	for ( size_t i=0; i<files.size(); ++i ) {
		todel = dir + "/" + files[i];
		
		// 子目录,递归调用
		if ( files[i][0] == '/' ) {
			 if ( !delete_dir(todel) )
				return false;
		}
		
		// 文件
		else if ( !delete_file(todel) )
			return false;
	}
	
	// 删除目录
	if ( rmdir(dir.c_str()) == 0 )
		return true;

	return false;
}
			
/// \ingroup waFileSystem
/// \fn bool move_dir( const string &srcdir, const string &destdir )
/// 移动目录
/// \param srcdir 原目录
/// \param destdir 目的目录
/// \retval true 操作成功
/// \retval false 失败
bool move_dir( const string &srcdir, const string &destdir ) {
	if ( rename_file(srcdir,destdir) )
		return true;

	// rename fail, copy and delete dir	
	if ( copy_dir(srcdir,destdir) ) {
		if ( delete_dir(srcdir) )
			return true;
	}

	return false;
}

/// \ingroup waFileSystem
/// \fn void lock_file( int fd, const int type )
/// 文件句柄锁函数，若文件已被锁则阻塞并等待
/// \param fd 文件句柄
/// \param type 锁模式，可选F_WRLCK、F_RDLCK、F_UNLCK
void lock_file( int fd, const int type ) {
	struct flock lck;
	lck.l_start = 0;
	lck.l_len = 0;
	lck.l_whence = SEEK_SET;
	
	lck.l_type = type;
	if ( type==F_WRLCK || type==F_RDLCK )
		lck.l_type = type;
	else
		lck.l_type = F_UNLCK;
	
	while ( fcntl(fd,F_SETLKW,&lck)==-1 && errno==EINTR );
	return;
}

/// \ingroup waFileSystem
/// \fn bool is_locked( int fd )
/// 判断文件句柄锁
/// \param fd 文件句柄
/// \retval true 文件已被锁
/// \retval false 文件未被锁
bool is_locked( int fd ) {
	struct flock lck;
	lck.l_start = 0;
	lck.l_len = 0;
	lck.l_whence = SEEK_SET;
	lck.l_type = F_WRLCK;

	while ( (fcntl(fd,F_GETLK,&lck))==-1 && errno==EINTR );

	if ( lck.l_type == F_UNLCK )
		return false;
	else
		return true;
}

/// \ingroup waFileSystem
/// \fn FILE* lock_open( const string &file, const char* mode, const int type )
/// 申请锁并打开文件，若文件已被锁则阻塞并等待
/// \param file 文件路径
/// \param mode 文件打开模式，与fopen()同参数意义相同
/// \param type 锁模式，可选F_WRLCK、F_RDLCK、F_UNLCK
/// \return 文件句柄，失败返回NULL
FILE* lock_open( const string &file, const char* mode, const int type ) {
	FILE *fp;
	mode_t mask = umask( 0 );
	int fd = open( file.c_str(), O_CREAT|O_EXCL|O_RDWR, 
				   S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH );
	if ( fd >= 0 ) {
		// file not exist, create success
		lock_file( fd, type );
		fp = fdopen( fd, mode );
	} else {
		// file exist
		fp = fopen( file.c_str(), mode );
		if ( fp == NULL )
			return NULL;
		lock_file( fileno(fp), type );
	}
	umask( mask );

	return fp;
}

} // namespace

