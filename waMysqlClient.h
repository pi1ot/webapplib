/// \file waMysqlClient.h
/// webapp::ysqlClient,webapp::MysqlData类头文件
/// MySQL数据库C++接口

// 编译参数:
// (CC) -I /usr/local/include/mysql/ -L /usr/local/lib/mysql -lmysqlclient -lm

#ifndef _WEBAPPLIB_MYSQLCLIENT_H_
#define _WEBAPPLIB_MYSQLCLIENT_H_ 

#include <string>
#include <vector>
#include <map>
#include <mysql.h>

using namespace std;

/// Web Application Library namaspace
namespace webapp {

/// \defgroup waMysqlClient waMysqlClient相关数据类型与全局函数

/// \ingroup waMysqlClient
/// \typedef MysqlDataRow 
/// MysqlData 数据行类型 (map<string,string>)
typedef map<string,string> MysqlDataRow;

/// SQL语句字符转义
string escape_sql( const string &str );

/// MySQL数据集类
class MysqlData {
	friend class MysqlClient;
	
	protected:
	
	/// 填充MysqlData数据
	bool fill_data( MYSQL *mysql );
	
	unsigned long _rows;
	unsigned int _cols;
	unsigned long _curpos;
	long _fetched;

	MYSQL_RES *_mysqlres;
	MYSQL_ROW _mysqlrow;
	MYSQL_FIELD *_mysqlfields;
	map<string,int> _field_pos;

	////////////////////////////////////////////////////////////////////////////
	public:

	/// MysqlData构造函数
	MysqlData():
	_rows(0), _cols(0), _curpos(0), _fetched(-1),
	_mysqlres(0), _mysqlfields(0)
	{};
	
	/// MysqlData析构函数
	virtual ~MysqlData();
	
	/// 返回指定位置的MysqlData数据
	/// \param row 行位置
	/// \param col 列位置
	/// \return 数据字符串
	inline string operator() ( const unsigned long row, const unsigned int col ) {
		return this->get_data( row, col );
	}
	/// 返回指定位置的MysqlData数据
	string get_data( const unsigned long row, const unsigned int col );

	/// 返回指定字段的MysqlData数据
	/// \param row 行位置
	/// \param field 字段名
	/// \return 数据字符串
	inline string operator() ( const unsigned long row, const string &field ) {
		return this->get_data( row, field );
	}
	/// 返回指定字段的MysqlData数据
	string get_data( const unsigned long row, const string &field );

	/// 返回指定位置的MysqlData数据行
	MysqlDataRow get_row( const long row = -1 );

	/// 返回MysqlData数据行数
	inline unsigned long rows() const {
		return _rows;
	}
	/// 返回MysqlData数据列数
	inline unsigned int cols() const {
		return _cols;
	}
	
	/// 返回字段位置
	int field_pos( const string &field );
	/// 返回字段名称
	string field_name( const unsigned int col ) const;

	////////////////////////////////////////////////////////////////////////////
	private:
	
	/// 禁止调用拷贝构造函数
	MysqlData( MysqlData &copy );
	/// 禁止调用拷贝赋值操作
	MysqlData& operator = ( const MysqlData& copy );
};

/// MySQL数据库连接类
class MysqlClient {
	public:
	
	/// Mysql默认构造函数
	MysqlClient():
	_connected(false)
	{};
	
	/// Mysql构造函数
	/// \param host MySQL主机IP
	/// \param user MySQL用户名
	/// \param pwd 用户口令
	/// \param database 要打开的数据库
	/// \param port 数据库端口，默认为0
	/// \param socket UNIX_SOCKET，默认为NULL
	MysqlClient( const string &host, const string &user, const string &pwd, 
		const string &database, const int port = 0, const char* socket = NULL ) 
	{
		this->connect( host, user, pwd, database, port, socket );
	}
	
	/// Mysql析构函数
	virtual ~MysqlClient() {
		this->disconnect();
	}
	
	/// 连接数据库
	bool connect( const string &host, const string &user, const string &pwd, 
		const string &database, const int port = 0, const char* socket = NULL );
	/// 断开数据库连接
	void disconnect();
	/// 判断是否连接数据库
	bool is_connected();
	
	/// 选择数据库
	bool select_db( const string &database );

	/// 执行SQL语句,取得查询结果
	bool query( const string &sqlstr, MysqlData &records );
	/// 执行SQL语句
	bool query( const string &sqlstr );
	/// 返回查询结果中指定位置的字符串值
	string query_val( const string &sqlstr, 
		const unsigned long row = 0, const unsigned int col = 0 );
    /// 返回查询结果中指定行
    MysqlDataRow query_row( const string &sqlstr, const unsigned long row = 0 );

	/// 上次查询动作所影响的记录条数
	unsigned long affected();
	/// 取得上次查询的一个AUTO_INCREMENT列生成的ID
	unsigned long last_id();
	
	/// 取得Mysql错误信息
	/// \return 返回错误信息字符串
	inline string error() {
		return string( mysql_error(&_mysql) );
	}
	/// 取得Mysql错误编号
	/// \return 返回错误信息编号
	inline unsigned int errnum() {
		return mysql_errno( &_mysql );
	}

	/// 取得更新信息
	string info();

	////////////////////////////////////////////////////////////////////////////
	private:
	
	/// 禁止调用拷贝构造函数
	MysqlClient( MysqlClient &copy );
	/// 禁止调用拷贝赋值操作
	MysqlClient& operator = ( const MysqlClient& copy );

	MYSQL _mysql;
	bool _connected;
};

} // namespace

#endif //_WEBAPPLIB_MYSQLCLIENT_H_

