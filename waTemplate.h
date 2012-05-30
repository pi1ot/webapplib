/// \file waTemplate.h
/// HTML模板处理类头文件
/// 支持条件、循环脚本的HTML模板处理类
/// 依赖于 waString
/// <a href="wa_template.html">使用说明文档及简单范例</a>

#ifndef _WEBAPPLIB_TMPL_H_
#define _WEBAPPLIB_TMPL_H_ 

#include <cstring>
#include <string>
#include <vector>
#include <map>
#include "waString.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {
	
/// 支持条件、循环脚本的HTML模板处理类
/// <a href="wa_template.html">使用说明文档及简单范例</a>
class Template {
	public:
	
	/// 默认构造函数
	Template(){};
	
	/// 构造函数
	/// \param tmpl_file 模板文件
	Template( const string tmpl_file ) {
		this->load( tmpl_file );
	}
	
	/// 构造函数
	/// \param tmpl_dir 模板目录
	/// \param tmpl_file 模板文件
	Template( const string tmpl_dir, const string tmpl_file ) {
		this->load( tmpl_dir, tmpl_file );
	}
	
	/// 析构函数
	virtual ~Template(){};
	
	/// \enum 输出时是否包括调试信息
	enum output_mode {
		/// 显示调试信息
		TMPL_OUTPUT_DEBUG,	
		/// 不显示
		TMPL_OUTPUT_RELEASE	
	};

	/// 读取HTML模板文件
	bool load( const string &tmpl_file );

	/// 读取模板
	/// \param tmpl_dir 模板目录
	/// \param tmpl_file 模板文件
	/// \retval true 读取成功
	/// \retval false 失败
	inline bool load( const string &tmpl_dir, const string &tmpl_file ) {
		return this->load( tmpl_dir + "/" + tmpl_file );
	}
	
	/// 设置HTML模板内容
	void tmpl( const string &tmpl );

	/// 设置替换规则
	void set( const string &name, const string &value );
	/// 设置替换规则
	/// \param name 模板域名称
	/// \param value 替换值
	inline void set( const string &name, const long value ) {
		this->set( name, itos(value) );
	}
	
	/// 新建循环
	void def_loop( const string &loop, const char* field_0, ... );
	/// 添加一行数据到循环
	void append_row( const string &loop, const char* value_0, ... );
	/// 添加一行指定格式的数据到循环
	void append_format( const string &loop, const char* format, ... );
	
	/// 清空所有替换规则
	void clear_set();

	/// 返回HTML字符串
	string html();
	/// 输出HTML到stdout
	void print( const output_mode mode = TMPL_OUTPUT_RELEASE );
	/// 输出HTML到文件
	bool print( const string &file, const output_mode mode = TMPL_OUTPUT_RELEASE,
		const mode_t permission = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH );
	
	////////////////////////////////////////////////////////////////////////////
	private:
	
	/// 读取指定位置的模板脚本类型及表达式
	int parse_script( const string &tmpl, const unsigned int pos,
		string &exp, int &type );

	/// 分析表达式的值
	string exp_value( const string &expression );

	/// 分析处理模板
	void parse( const string &tmpl, ostream &output );
	
	/// 检查条件语句表达式是否成立
	bool compare( const string &exp );
	
	/// 检查条件是否成立
	bool check_if( const string &exp );

	/// 处理条件类型模板
	size_t parse_if( const string &tmpl, ostream &output, 
		const bool parent_state, const string &parsed_exp,
		const int parsed_length );

	/// 返回字段位置
	int field_pos( const string &loop, const string &field );

	/// 检查循环语句是否有效
	bool check_loop( const string &loopname );
	
	/// 返回循环中指定位置字段的值
	string loop_value( const string &field );

	/// 处理循环类型模板
	size_t parse_loop( const string &tmpl, ostream &output, 
		const bool parent_state, const string &parsed_exp,
		const int parsed_length );
							
	/// 模板分析错误纪录
	void error_log( const size_t lines, const string &error );
	/// 模板分析纪录
	void parse_log( ostream &output );

	// 数据定义
	typedef vector<string> strings;		// 字符串列表
	typedef struct {					// 循环模板设置结构
		int cols;						// 循环字段数量
		int rows;						// 循环数据行数
		int cursor;						// 当前光标位置
		strings fields;					// 循环字段定义列表
		map<string,int> fieldspos;		// 循环字段位置,for speed
		vector<strings> datas;			// 循环数据
	} tmpl_loop;

	// 模板数据
	String _tmpl;						// HTML模板内容
	map<string,string> _sets;			// 替换规则列表 <模板域名称,模板域值>
	map<string,tmpl_loop> _loops;		// 循环替换规则列表 <循环名称,循环模板设置结构>
	
	// 分析过程数据
	string _loop;						// 当前循环名称
	int _cursor;						// 当前循环光标位置
	int _lines;							// 已处理模板行数

	string _tmplfile;					// HTML模板文件名
	char _date[15];						// 当前日期
	char _time[15];						// 当前时间
	output_mode _debug;					// 分析模式
	multimap<int,string> _errlog;		// 分析错误纪录 <错误位置行数,错误描述信息>
};

// 模板语法格式定义
const char TMPL_BEGIN[]		= "{{";		const int TMPL_BEGIN_LEN 	= strlen(TMPL_BEGIN);
const char TMPL_END[]		= "}}";		const int TMPL_END_LEN 		= strlen(TMPL_END);
const char TMPL_SUBBEGIN[]	= "(";		const int TMPL_SUBBEGIN_LEN = strlen(TMPL_SUBBEGIN);
const char TMPL_SUBEND[]	= ")";		const int TMPL_SUBEND_LEN 	= strlen(TMPL_SUBEND);
const char TMPL_SPLIT[]		= ",";		const int TMPL_SPLIT_LEN 	= strlen(TMPL_SPLIT);
const char TMPL_NEWLINE[]	= "\n";		const int TMPL_NEWLINE_LEN 	= strlen(TMPL_NEWLINE);

const char TMPL_VALUE[]		= "$";		const int TMPL_VALUE_LEN 	= strlen(TMPL_VALUE);
const char TMPL_DATE[]		= "%DATE";	const int TMPL_DATE_LEN 	= strlen(TMPL_DATE);
const char TMPL_TIME[]		= "%TIME";	const int TMPL_TIME_LEN 	= strlen(TMPL_TIME);
const char TMPL_SPACE[]		= "%SPACE";	const int TMPL_SPACE_LEN	= strlen(TMPL_SPACE);
const char TMPL_BLANK[]		= "%BLANK";	const int TMPL_BLANK_LEN 	= strlen(TMPL_BLANK);
                                        
const char TMPL_LOOP[]		= "#FOR";	const int TMPL_LOOP_LEN 	= strlen(TMPL_LOOP);
const char TMPL_ENDLOOP[]	= "#ENDFOR";const int TMPL_ENDLOOP_LEN = strlen(TMPL_ENDLOOP);
const char TMPL_LOOPVALUE[]	= ".$";		const int TMPL_LOOPVALUE_LEN = strlen(TMPL_LOOPVALUE);
const char TMPL_LOOPSCOPE[]	= "@";		const int TMPL_LOOPSCOPE_LEN = strlen(TMPL_LOOPSCOPE);
const char TMPL_CURSOR[]	= "%CURSOR";const int TMPL_CURSOR_LEN	= strlen(TMPL_CURSOR);
const char TMPL_ROWS[]		= "%ROWS";	const int TMPL_ROWS_LEN		= strlen(TMPL_ROWS);

const char TMPL_IF[]		= "#IF";	const int TMPL_IF_LEN 		= strlen(TMPL_IF);
const char TMPL_ELSIF[]		= "#ELSIF";	const int TMPL_ELSIF_LEN 	= strlen(TMPL_ELSIF);
const char TMPL_ELSE[]		= "#ELSE";	const int TMPL_ELSE_LEN 	= strlen(TMPL_ELSE);
const char TMPL_ENDIF[]		= "#ENDIF";	const int TMPL_ENDIF_LEN 	= strlen(TMPL_ENDIF);

// 比较操作符定义
const char TMPL_AND[]		= "AND";	const int TMPL_AND_LEN 		= strlen(TMPL_AND);
const char TMPL_OR[]		= "OR";		const int TMPL_OR_LEN 		= strlen(TMPL_OR);
const char TMPL_EQ[]		= "==";		const int TMPL_EQ_LEN 		= strlen(TMPL_EQ);
const char TMPL_NE[]		= "!=";		const int TMPL_NE_LEN 		= strlen(TMPL_NE);
const char TMPL_LE[]		= "<=";		const int TMPL_LE_LEN 		= strlen(TMPL_LE);
const char TMPL_LT[]		= "<";		const int TMPL_LT_LEN 		= strlen(TMPL_LT);
const char TMPL_GE[]		= ">=";		const int TMPL_GE_LEN 		= strlen(TMPL_GE);
const char TMPL_GT[]		= ">";		const int TMPL_GT_LEN 		= strlen(TMPL_GT);

// Htt::format_row()格式定义
const char TMPL_FMTSTR[]	= "%s";		const int TMPL_FMTSTR_LEN 	= strlen(TMPL_FMTSTR);
const char TMPL_FMTINT[]	= "%d";		const int TMPL_FMTINT_LEN 	= strlen(TMPL_FMTINT);

// 脚本语句类型
enum tmpl_scripttype {
	TMPL_S_VALUE,	
	TMPL_S_LOOPVALUE,	
	TMPL_S_LOOP,	
	TMPL_S_ENDLOOP,	
	TMPL_S_IF,		
	TMPL_S_ELSIF,	
	TMPL_S_ELSE,		
	TMPL_S_ENDIF,
	TMPL_S_CURSOR,
	TMPL_S_ROWS,
	TMPL_S_DATE,
	TMPL_S_TIME,
	TMPL_S_SPACE,
	TMPL_S_BLANK,
	TMPL_S_UNKNOWN
};

// 逻辑运算类型
enum tmpl_logictype {
	TMPL_L_NONE,	
	TMPL_L_AND,
	TMPL_L_OR
};

// 比较运算类型
enum tmpl_cmptype {
	TMPL_C_EQ,
	TMPL_C_NE,
	TMPL_C_LE,
	TMPL_C_LT,
	TMPL_C_GE,
	TMPL_C_GT
};

} // namespace

#endif //_WEBAPPLIB_TMPL_H_

