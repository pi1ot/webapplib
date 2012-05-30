/// \file waConfigFile.cpp
/// INI格式配置文件解析类实现文件

#include <fstream>
#include <set>
#include <algorithm>
#include "waString.h"
#include "waTextFile.h"
#include "waConfigFile.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {
	
/// 读取解析配置文件
/// \param file 配置文件路径名
/// \retval true 解析成功
/// \retval false 解析失败
bool ConfigFile::load( const string &file ) {
	if ( file == "" ) return false;
	_file = file;
	
	size_t pos;
	bool line_continue = false;
	String curr_block, curr_name, curr_value;
	
	String line;
	TextFile config( file );
	while ( config.next_line(line) ) {
		line.trim();
		
		// multi-line continue 
		if ( line_continue ) {
			if ( line.right(2) == " \\" ) {
				// continue...					
				line_continue = true;
				line = line.substr( 0, line.length()-2 );
				line.trim();
				curr_value += line;
			} else {
				// end
				line_continue = false;
				curr_value += line;
				_config[ curr_block ][ curr_name ] = curr_value;
			}
		}
		
		// comment line
		else if ( line[0] == '#' ) {
			continue;
		}
		
		// block define
		else if ( line[0]=='[' && line[line.length()-1]==']' ) {
			curr_block = line.substr( 1, line.length()-2 );
			curr_block.trim();
			if ( curr_block != "" ) {
				value_def null_value;
				_config[ curr_block ] = null_value;
			}
		}
		
		// value define
		else if ( (pos=line.find("=")) != line.npos ) {
			String name = line.substr( 0, pos );
			String value = line.substr( pos+1 );
			name.trim();
			value.trim();
			if ( name == "" )
				continue;

			if ( value.right(2) == " \\" ) {
				// multi-line						
				line_continue = true;
				value = value.substr( 0, value.length()-2 );
				value.trim();
				curr_name = name;
				curr_value = value;
			} else {
				// single line
				line_continue = false;
				_config[ curr_block ][ name ] = value;
			}
		}
		
		// unknown
		else {
			continue; // ignore
		}
	}

	config.close();	
	return true;
}

/// 保存配置文件
/// \param file 配置文件路径名，默认为空则使用读取文件参数
/// \retval true 保存成功
/// \retval false 保存失败
bool ConfigFile::save( const string &file ) {
	string config_file = _file;
	if ( file != "" )
		config_file = file;
	
	// for each block
	String content;
	content.reserve( 1024 );
	
	_biter = _config.begin();
	for ( ; _biter!=_config.end(); ++_biter ) {
		if ( _biter->first != "" )
			content += "[" + (_biter->first) + "]\n";
			
		// for each value
		value_def valuelist = _biter->second;
		_viter = valuelist.begin();
		for ( ; _viter!=valuelist.end(); ++_viter )
			content += (_viter->first) + " = " + (_viter->second) + "\n";
	}
	
	_unsaved = !( content.save_file(config_file) );
	return _unsaved;
}

/// 检查配置项是否存在	
/// \param block 配置块名，为空表示全局配置块
/// \param name 配置项名
/// \retval true 存在
/// \retval false 不存在
bool ConfigFile::value_exist( const string &block, const string &name ) {
	if ( name == "" )
		return false;	
	
	// locate block
	_biter = _config.find( block );
	if ( _biter != _config.end() ) {
		// locate name
		value_def valuelist = _biter->second;
		_viter = valuelist.find( name );
		if ( _viter != valuelist.end() )
			return true;
	}
	
	return false;
}

/// 检查配置块是否存在
/// \param block 配置块名，为空表示全局配置块
/// \retval true 存在
/// \retval false 不存在
bool ConfigFile::block_exist( const string &block ) {
	if ( block == "" )
		return true;
	
	_biter = _config.find( block );
	if ( _biter != _config.end() )
		return true;
	else
		return false;
}

/// 读取配置项参数值
/// \param block 配置块名，为空表示全局配置块
/// \param name 配置项名
/// \param default_value 默认参数值，配置项不存在时返回
/// \return 指定配置项参数值
string ConfigFile::get_value( const string &block, const string &name, 
						  const string &default_value ) {
	if ( name == "" )
		return default_value;
	         
	// locate block
	_biter = _config.find( block );
	if ( _biter == _config.end() )
		return default_value;
	
	// locate value
	value_def valuelist = _biter->second;
	_viter = valuelist.find( name );
	if ( _viter == valuelist.end() )
		return default_value;
	
	return _viter->second;
}

/// 读取指定配置块的全部配置项参数值
/// \param block 配置块名，为空表示全局配置块
/// \return 指定配置块的全部配置项参数值列表
map<string,string> ConfigFile::get_block( const string &block ) {
	value_def valuelist;	
	if ( block_exist(block) )
		valuelist = _config[block];
	return valuelist;
}

/// 读取全部配置块列表		
/// \return 全部配置块列表，包括全局配置块（block值为空）
vector<string> ConfigFile::block_list() {
	vector<string> blocklist;
	_biter = _config.begin();
	for ( ; _biter!=_config.end(); _biter++ )
		blocklist.push_back( _biter->first );
	return blocklist;
}
	
/// 更新配置项
/// \param block 配置块名，为空表示全局配置块
/// \param name 配置项名
/// \param value 配置参数值
/// \retval true 更新成功
/// \retval false 更新失败
bool ConfigFile::set_value( const string &block, const string &name, 
	const string &value ) 
{
	if ( name == "" )
		return false;
	_config[ block ][ name ] = value;
	_unsaved = true;
	return _unsaved;
}

/// 更新指定配置块的配置项列表
/// \param block 配置块名，为空表示全局配置块
/// \param valuelist 配置参数值对列表
/// \retval true 更新成功
/// \retval false 更新失败
bool ConfigFile::set_block( const string &block, 
	const map<string,string> &valuelist ) 
{
	if ( valuelist.size() <= 0 )
		return false;
	
	value_def::const_iterator i = valuelist.begin();
	for ( ; i!=valuelist.end(); ++i )
		_config[ block ][ i->first ] = i->second;
	
	_unsaved = true;
	return _unsaved;
}

/// 删除配置项
/// \param block 配置块名，为空表示全局配置块
/// \param name 配置项名
void ConfigFile::del_value( const string &block, const string &name ) {
	_biter = _config.find( block );
	if ( _biter != _config.end() ) {
		_viter = (_biter->second).find( name );
		if ( _viter != (_biter->second).end() ) {
			(_biter->second).erase( _viter );
			_unsaved = true;
			return;
		}			
	}
}

/// 删除配置块
/// \param block 配置块名，为空表示全局配置块
void ConfigFile::del_block( const string &block ) {
	_biter = _config.find( block );
	if ( _biter != _config.end() ) {
		_config.erase( _biter );
		_unsaved = true;
		return;
	}
}

} // namespace

