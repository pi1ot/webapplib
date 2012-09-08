/// \file waUtility.h
/// 系统调用工具函数头文件
/// 常用系统调用和工具函数
/// 依赖于 webapp::String, webapp::DateTime

#ifndef _WEBAPPLIB_UTILITY_H_
#define _WEBAPPLIB_UTILITY_H_ 

#include <string>
#include <map>

using namespace std;

/// Web Application Library namaspace
namespace webapp {

/// \defgroup waUtility waUtility系统调用工具函数库

// 全角半角字符转换表
#define SDBC_TABLE_SIZE		172
static const char SBC_TABLE[SDBC_TABLE_SIZE][3] = {
	"１", "２", "３", "４", "５", "６", "７", "８", "９", "０",
	"一", "二", "三", "四", "五", "六", "七", "八", "九", "零",
	"壹", "贰", "叁", "肆", "伍", "陆", "柒", "捌", "玖", "零",
	"⒈", "⒉", "⒊", "⒋", "⒌", "⒍", "⒎", "⒏", "⒐", "⒑",
	"⑴", "⑵", "⑶", "⑷", "⑸", "⑹", "⑺", "⑻", "⑼", "⑽",
	"①", "②", "③", "④", "⑤", "⑥", "⑦", "⑧", "⑨", "⑩",
	"㈠", "㈡", "㈢", "㈣", "㈤", "㈥", "㈦", "㈧", "㈨", "㈩",
	"ａ", "ｂ", "ｃ", "ｄ", "ｅ", "ｆ", "ｇ", "ｈ", "ｉ", "ｊ", "ｋ", "ｌ", "ｍ", "ｎ", "ｏ", "ｐ" ,"ｑ", "ｒ", "ｓ", "ｔ", "ｕ", "ｖ", "ｗ", "ｘ", "ｙ", "ｚ",
	"Ａ", "Ｂ", "Ｃ", "Ｄ", "Ｅ", "Ｆ", "Ｇ", "Ｈ", "Ｉ", "Ｊ", "Ｋ", "Ｌ", "Ｍ", "Ｎ", "Ｏ", "Ｐ" ,"Ｑ", "Ｒ", "Ｓ", "Ｔ", "Ｕ", "Ｖ", "Ｗ", "Ｘ", "Ｙ", "Ｚ",
	"－", "＝", "［", "］", "、", "；", "｀", "＇", "‘", "’", "，", "。", "／", "～", "！", "·", "＃",
	"￥", "％", "︿", "＆", "＊", "※", "（", "）", "—", "＋", "｛", "｝", "｜", "：", "《", "》", "？",
	"…", "＼", "．", "＠", "＄", "＿", "“", "＂", "”", "＜", "＞", "　", "〈", "〉", "【", "】"
};
static const char DBC_TABLE[SDBC_TABLE_SIZE] = {
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'-', '=', '[', ']', ',', ';', '`', '\'','`', '\'',',', '.', '/', '~', '!', '.', '#',
	'$', '%', '^', '&', '*', '*', '(', ')', '-', '+', '{', '}', '|', ':', '<', '>', '?',
	'.', '\\','.', '@', '$', '_', '"', '"', '"', '<', '>', ' ', '<', '>', '[', ']'
};

/// \ingroup waUtility 
/// \enum 提取正文函数过滤选项
enum extract_option {
	/// 过滤英文字母
	EXTRACT_ALPHA	= 2,	
	/// 过滤阿拉伯数字
	EXTRACT_DIGIT	= 4,
	/// 过滤半角标点	
	EXTRACT_PUNCT	= 8,	
	/// 过滤空白  
	EXTRACT_SPACE	= 16,	
	/// 过滤HTML代码
	EXTRACT_HTML	= 32	
};

/// \ingroup waUtility 
/// \def EXTRACT_ALL 
/// 提取正文函数过滤选项，过滤全部（字母、数字、标点、空白、HTML）
#define EXTRACT_ALL	(EXTRACT_ALPHA|EXTRACT_DIGIT|EXTRACT_PUNCT|EXTRACT_SPACE|EXTRACT_HTML)

/// 返回字符串HASH值，基于DJB HASH算法
size_t string_hash( const string &str );

/// 全文词表替换，兼容GBK汉字
string replace_text( const string &text, const map<string,string> &replace );

/// 提取HTML代码正文
string extract_html( const string &html );

/// 全角半角字符转换并提取正文
string extract_text( const string &text, const int option=EXTRACT_ALL, 
	const size_t len=0 );

/// 追加日志记录
void file_logger( const string &file, const char *format, ... );

/// 追加日志记录
void file_logger( FILE *fp, const char *format, ... );

/// 执行命令并返回命令输出结果
string system_command( const string &cmd );

/// 返回指定网卡设备绑定的IP地址
string host_addr( const string &interface="eth0" );

} // namespace

#endif //_WEBAPPLIB_UTILITY_H_

