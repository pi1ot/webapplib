/// \file waConfigFile.h
/// INI格式配置文件解析类头文件
/// 依赖于 webapp::String, webapp::TextFile

#ifndef _WEBAPPLIB_CONFIGFILE_H_
#define _WEBAPPLIB_CONFIGFILE_H_

#include <string>
#include <vector>
#include <map>

using namespace std;

/// Web Application Library namaspace
namespace webapp {

/// INI格式配置文件解析类
class ConfigFile
{
	public:
	
	/// 默认构造函数
	ConfigFile(): _file(""), _unsaved(false) {};
	
	/// 参数为配置文件名的构造函数
	ConfigFile( const string &file )
	: _file(""), _unsaved(false) {
		this->load( file );
	}
	
	/// 析构函数
	~ConfigFile() {
		if ( _unsaved ) this->save();
	};

	////////////////////////////////////////////////////////////////////////////
	/// 读取解析配置文件
	bool load( const string &file );
	
	/// 保存配置文件
	bool save( const string &file = "" );

	/// 检查配置项是否存在	
	bool value_exist( const string &block, const string &name );
	
	/// 检查配置块是否存在
	bool block_exist( const string &block );

	////////////////////////////////////////////////////////////////////////////
	/// 读取配置项参数值
	inline string operator[] ( const string &name ) {
		return this->get_value( "", name );
	}

	/// 读取配置项参数值
	string get_value( const string &block, const string &name, const string &default_value = "" );

	/// 读取指定配置块的全部配置项参数值
	map<string,string> get_block( const string &block );

	/// 读取全部配置块列表					
	vector<string> block_list();
	
	////////////////////////////////////////////////////////////////////////////
	/// 更新配置项
	inline bool set_value( const string &name, const string &value ) {
		return this->set_value( "", name, value );
	}

	/// 更新配置项
	bool set_value( const string &block, const string &name, const string &value );
	
	/// 更新指定配置块的配置项列表
	bool set_block( const string &block, const map<string,string> &valuelist );

	////////////////////////////////////////////////////////////////////////////
	/// 删除配置项
	void del_value( const string &block, const string &name );
	
	/// 删除配置块
	void del_block( const string &block );
	
	////////////////////////////////////////////////////////////////////////////
	private:
	
	typedef map<string,string> value_def; // name -> value
	typedef map<string,value_def> block_def; // block -> ( name -> value )

	string _file;
	bool _unsaved;
	block_def _config;
	block_def::iterator _biter;
	value_def::iterator _viter;
};

} // namespace

#endif //_WEBAPPLIB_CONFIGFILE_H_ 

