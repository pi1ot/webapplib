/// \file waMysqlClient.cpp
/// webapp::MysqlClient,webapp::MysqlData类实现文件

// 编译参数:
// (CC) -I /usr/local/include/mysql/ -L /usr/local/lib/mysql -lmysqlclient -lm

#include <cstring>
#include "waMysqlClient.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {

/// \ingroup waMysqlClient
/// \fn string escape_sql( const string &str )
/// SQL语句字符转义
/// \param 要转换的SQL字符串
/// \return 转义过的字符串
string escape_sql( const string &str ) {
	char *p = new char[str.length()*2+1];
	mysql_escape_string( p, str.c_str(), str.length() );
	string s = p;
	delete[] p;
	return s;
}

////////////////////////////////////////////////////////////////////////////
// MysqlData

/// MysqlData析构函数
MysqlData::~MysqlData() {
	if ( _mysqlres != NULL )
		mysql_free_result( _mysqlres );
	_mysqlres = 0;
	_mysqlfields = 0;
}

/// 返回指定位置的MysqlData数据
/// \param row 数据行位置,默认为0
/// \param col 数据列位置,默认为0
/// \return 返回数据,不存在则返回空字符串
string MysqlData::get_data( const size_t row, const size_t col ) {
	if( _mysqlres!=NULL && row<_rows && col<_cols ) {
		if ( row != _fetched ) {
			if ( row != _curpos+1 ) {
				mysql_data_seek( _mysqlres, row );
			}
			_mysqlrow = mysql_fetch_row( _mysqlres );
			_fetched = row; 
		}
		
		if ( _mysqlrow!=NULL && _mysqlrow[col]!=NULL ) {
			_curpos = row; // log current cursor
			return  string( _mysqlrow[col] );
		}
	}
	return string( "" );
}

/// 返回指定字段的MysqlData数据
/// \param row 行位置
/// \param field 字段名
/// \return 数据字符串,不存在返回空字符串
string MysqlData::get_data( const size_t row, const string &field ) {
	int col = this->field_pos( field );
	if ( col != -1 )
		return this->get_data( row, col );
	else
		return string( "" );
}

/// 返回指定位置的MysqlData数据行
/// \param row 数据行位置,默认为0即第一行
/// \return 返回值类型为MysqlDataRow,即map<string,string>
MysqlDataRow MysqlData::get_row( const size_t row ) {
	MysqlDataRow datarow;
	string field;
		
	if( _mysqlres!=NULL && row<_rows ) {
		if ( row != _curpos ) {
			if ( row != _curpos+1 ) {
				mysql_data_seek( _mysqlres, row );
			}
			_mysqlrow = mysql_fetch_row( _mysqlres );
		}
		
		if ( _mysqlrow != NULL ) {
			_curpos = row; // log current cursor
			for ( size_t i=0; i<_cols; ++i ) {
				field = this->field_name( i );
				if ( field!="" && _mysqlrow[i]!=NULL ) {
					datarow.insert( MysqlDataRow::value_type(field,_mysqlrow[i]) );
				}
			}
		}
	}
	
	return datarow;
}

/// 填充MysqlData数据
/// \param mysql MYSQL*参数
/// \retval true 成功
/// \retval false 失败
bool MysqlData::fill_data( MYSQL *mysql ) {
	if ( mysql == NULL )
		return false;
	
	// clean		
	if ( _mysqlres != NULL )
		mysql_free_result( _mysqlres );
	_mysqlres = 0;
	_curpos = 0; // return to first position
	_field_pos.clear(); // clean field pos cache

	// fill data
	_mysqlres = mysql_store_result( mysql );
	if ( _mysqlres != NULL ) {
		_rows = mysql_num_rows( _mysqlres );
		_cols = mysql_num_fields( _mysqlres );
		_mysqlfields = mysql_fetch_fields( _mysqlres );
		
		// init first data
		mysql_data_seek( _mysqlres, 0 );
		_mysqlrow = mysql_fetch_row( _mysqlres );
		_fetched = 0;		
		
		return true;
	}
	return false;
}

/// 返回字段位置
/// \param field 字段名
/// \return 若数据结果中存在该字段则返回字段位置,否则返回-1
int MysqlData::field_pos( const string &field ) {
	if ( _mysqlfields==0 || field=="" )
		return -1;
	
	// check cache
	if ( _field_pos.find(field) != _field_pos.end() )
		return _field_pos[field];

	for( size_t i=0; i<_cols; ++i ) {
		if ( strcmp(field.c_str(),_mysqlfields[i].name) == 0 ) {
			_field_pos[field] = i;
			return i;
		}
	}
	_field_pos[field] = -1;
	return -1;
}

/// 返回字段名称
/// \param col 字段位置
/// \return 若数据结果中存在该字段则返回字段名称,否则返回空字符串
string MysqlData::field_name( size_t col ) const {
	if ( _mysqlfields!=0 && col<=_cols )
		return string( _mysqlfields[col].name );
	else
		return string( "" );
}

////////////////////////////////////////////////////////////////////////////
// MysqlClient

/// 连接数据库
/// \param host MySQL主机IP
/// \param user MySQL用户名
/// \param pwd 用户口令
/// \param database 要打开的数据库
/// \param port 数据库端口，默认为0
/// \param socket UNIX_SOCKET，默认为NULL
/// \retval true 成功
/// \retval false 失败
bool MysqlClient::connect( const string &host, const string &user, const string &pwd, 
	const string &database, const int port, const char* socket ) 
{
	this->disconnect();
	
	if ( mysql_init(&_mysql) ) {
		if ( mysql_real_connect( &_mysql, host.c_str(), user.c_str(),
			pwd.c_str(), database.c_str(), port, socket, CLIENT_COMPRESS ) )
			_connected = true;
	}
	
	return _connected;
}

/// 断开数据库连接
void MysqlClient::disconnect() {
	if( _connected ) {
		mysql_close( &_mysql );
		_connected = false;
	}
}

/// 判断是否连接数据库
/// \retval true 连接
/// \retval false 断开
bool MysqlClient::is_connected() {
	if ( _connected ) {
		if ( mysql_ping(&_mysql) == 0 )
			_connected = true;
		else
			_connected = false;
	}
	
	return _connected;
}

/// 选择数据库
/// \param database 数据库名
/// \retval true 成功
/// \retval false 失败
bool MysqlClient::select_db( const string &database ) {
	if ( _connected && mysql_select_db(&_mysql,database.c_str())==0 )
		return true;
	else
		return false;
}

/// 执行SQL语句,取得查询结果
/// \param sqlstr 要执行的SQL语句
/// \param records 保存数据结果的MysqlData对象
/// \retval true 成功
/// \retval false 失败
bool MysqlClient::query( const string &sqlstr, MysqlData &records ) {
	if ( _connected && mysql_real_query(&_mysql,sqlstr.c_str(),sqlstr.length())==0 ) {
		if( records.fill_data(&_mysql) )
			return true;
	}
	return false;
}

/// 执行SQL语句
/// \param sqlstr 要执行的SQL语句
/// \retval true 成功
/// \retval false 失败
bool MysqlClient::query( const string &sqlstr ) {
	if ( _connected && mysql_real_query(&_mysql,sqlstr.c_str(),sqlstr.length())==0 )
		return true;			
	else
		return false;
}

/// 返回查询结果中指定位置的字符串值
/// \param sqlstr SQL查询字符串
/// \param row 数据行位置,默认为0
/// \param col 数据列位置,默认为0
/// \return 查询成功返回字符串,否则返回空字符串
string MysqlClient::query_val( const string &sqlstr, const size_t row, 
	const size_t col ) 
{
	MysqlData data;
	if ( this->query(sqlstr,data) ) {
		if ( data.rows()>row && data.cols()>col )
			return data(row,col);
	}
	return string( "" );
}

/// 返回查询结果中指定行
/// \param sqlstr SQL查询字符串
/// \param row 数据行位置,默认为0
/// \return 返回值类型为MysqlDataRow,即map<string,string>
MysqlDataRow MysqlClient::query_row( const string &sqlstr, const size_t row ) {
    MysqlData data;
    MysqlDataRow datarow;
    if ( this->query(sqlstr,data) ) {
        if ( row < data.rows() )
            datarow = data.get_row( row );
    }

    return datarow;
}

/// 上次查询动作所影响的记录条数
/// \return 返回记录条数,类型size_t
size_t MysqlClient::affected() {
	if ( _connected )
		return mysql_affected_rows( &_mysql );
	else
		return 0;
}

/// 取得上次查询的一个AUTO_INCREMENT列生成的ID
/// 一个Mysql表只能有一个AUTO_INCREMENT列,且必须为索引
/// \return 返回生成的ID
size_t MysqlClient::last_id() {
	if ( _connected )
		return mysql_insert_id( &_mysql );
	else
		return 0;
}

/// 取得更新信息
/// \return 返回更新信息
string MysqlClient::info() {
	if ( _connected )
		return string( mysql_info(&_mysql) );
	else
		return string( "" );
}

} // namespace

