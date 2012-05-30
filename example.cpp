/// \file example.cpp
/// 代码示例文件，演示一个简单CGI流程

#include <iostream>
#include "webapplib.h"

using namespace webapp;

int main() {
	/***************************************************************************
	演示完整的CGI应用程序流程,模拟WEB查询动作	
	1、读取CGI参数和Cookie数据（String、Cgi、Cookie）
	2、使用读取到的参数调用权限检查接口（ConfigFile、HttpClient）
	3、使用读取到的参数查询数据库（MysqlClient）
	4、使用日志文件记录用户请求（FileSystem、Utility、Encode）
	5、更新用户端Cookie（Cookie、DateTime）
	6、显示HTML页面（Template）
	***************************************************************************/	

	////////////////////////////////////////////////////////////////////////////
	// 1、读取CGI参数和Cookie数据（Cgi、Cookie）
	
	Cgi cgi;
	Cookie cookie;
	String username = cgi["username"];
	String usercookie = cookie["usercookie"];
	
	/*提示 webapp::Cgi在读不到CGI环境变量时会运行在调试模式，提示输入CGI参数值*/
	
	////////////////////////////////////////////////////////////////////////////
	// 2、使用读取到的参数调用权限检查接口（ConfigFile、HttpClient）
	
	ConfigFile conf( "example.conf" );
	String check_interface = conf["check_interface"];
	
	cout << "---------------------------------------------------------" << endl;
	cout << "check user privilege from:" << check_interface << endl;
	
	HttpClient www;
	www.request( check_interface + "?username=" + username );
	if ( www.done() && www.content()=="CHECK_PASS" ) {
		cout << "check pass" << endl;
	} else {
		cout << "check fail" << endl;
	}
	
	////////////////////////////////////////////////////////////////////////////
	// 3、使用读取到的参数查询数据库（MysqlClient）
	String value;
	#ifndef _WEBAPPLIB_NOMYSQL

	String sql;
	sql.sprintf( "SELECT value FROM table WHERE user='%s'", escape_sql(username).c_str() );
	
	MysqlClient mysqlclient;
	MysqlData mysqldata;

	mysqlclient.connect( "example.mysql.com", "user", "pwd", "database" );
	if ( mysqlclient.is_connected() ) {
		if ( mysqlclient.query(sql,mysqldata) ) {
			value = mysqldata( 0, "value" );
		} else {
			cout << mysqlclient.error() << endl;
		}
	}

	#endif //_WEBAPPLIB_NOMYSQL

	////////////////////////////////////////////////////////////////////////////
	// 4、使用日志文件记录用户请求（FileSystem、Utility、Encode）
	
	String log_path = "/tmp/";
	String log_file = log_path + "/logfile.txt";
	if ( !file_exist(log_path) || !is_dir(log_path) ) {
		make_dir( log_path );
	}

	file_logger( log_file, "username:%s", username.c_str() );
	file_logger( log_file, "usermd5:%s", md5_encode(username).c_str() );
	
	cout << "---------------------------------------------------------" << endl;
	String file_content;
	file_content.load_file( log_file );
	cout << file_content << endl;

	////////////////////////////////////////////////////////////////////////////
	// 5、更新用户端Cookie（Cookie、DateTime）
	
	DateTime now;
	DateTime expires = now + ( TIME_ONE_DAY*3 ); // Cookie有效期为三天
	cookie.set_cookie( "username", username, expires.gmt_datetime() );
	
	////////////////////////////////////////////////////////////////////////////
	// 6、显示HTML页面（Template）
	
	Template page("example.tmpl");
	page.set( "username", username );
	page.set( "value", value );
	
	// 显示查询结果
	cout << "---------------------------------------------------------" << endl;
	http_head();
	page.print();
}

