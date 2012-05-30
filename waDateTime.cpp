/// \file waDateTime.cpp
/// webapp::DateTime类实现文件

#include <cstdio>
#include <cstring>
#include <climits>
#include "waDateTime.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {
	
/// 以当前时间设置对象
void DateTime::set() {
	_time = ::time( 0 );
	localtime_r( &_time, &_tm );
}

/// 以 time_t 参数设置对象
/// \param tt time_t类型参数
void DateTime::set( const time_t &tt ) {
	time_t _tt = tt;
	if ( tt < 0 ) _tt = 0;
	if ( tt > LONG_MAX ) _tt = LONG_MAX;
	
	_time = _tt;
	localtime_r( &_time, &_tm );
}

/// 以指定时间设置对象
/// 若参数不是有效日期时间,则设置为系统初始时间（1970/1/1）
/// 若参数日期时间不存在,则设置为顺延有效时间（非闰年2/29视为3/1）
/// \param year 年
/// \param mon 月
/// \param mday 日
/// \param hour 时,默认为0
/// \param min 分,默认为0
/// \param src 秒,默认为0
void DateTime::set( const int year, const int mon, const int mday, 
	const int hour, const int min, const int sec ) 
{
	int _year = year;
	int _mon = mon;
	int _mday = mday;
	int _hour = hour;
	int _min = min;
	int _sec = sec;

	// confirm
	if ( _year<1 || _year>2038 )	_year = 1970;
	if ( _mon<1  || _mon>12 ) 		_mon  = 1;
	if ( _mday<1 || _mday>31 )		_mday = 1;
	if ( _hour<0 || _hour>23 )		_hour = 0;
	if ( _min<0  || _min>59 ) 		_min  = 0;
	if ( _sec<0  || _sec>59 ) 		_sec  = 0;
	
	_tm.tm_year = _year-1900;
	_tm.tm_mon = _mon-1;
	_tm.tm_mday = _mday;
	_tm.tm_hour = _hour;
	_tm.tm_min = _min;
	_tm.tm_sec = _sec;
	_tm.tm_isdst = -1;
	_time = mktime( &_tm );
}

/// 以 tm 结构参数设置对象
/// \param st struct tm类型参数
void DateTime::set( const tm &st ) {
	this->set( st.tm_year+1900, st.tm_mon+1, st.tm_mday,
		st.tm_hour, st.tm_min, st.tm_sec );
}

/// 以 DateTime 参数设置对象
/// \param date Date类型参数
void DateTime::set( const DateTime &date ) {
	this->set( date.value() );
}

/// 以"YYYY-MM-DD HH:MM:SS"格式字符串设置对象
/// 若字符串格式错误或者时间值错误则设置为当前时间
/// \param datetime "YYYY-MM-DD HH:MM:SS"格式日期时间字符串
/// \param datemark 日期分隔字符,默认为"-"
/// \param dtmark 日期时间分隔字符,默认为" ",不能与datemark或timemark相同
/// \param timemark 时间分隔字符,默认为":"
void DateTime::set( const string &datetime, const string &datemark, 
	const string &dtmark, const string &timemark ) 
{
	// init struct tm
	struct tm tm;
	tm.tm_isdst = -1;

	// init format
	string fmt;
	if ( datetime.find(dtmark) != datetime.npos )
		fmt = "%Y" + datemark + "%m" + datemark + "%d" + dtmark + 
			  "%H" + timemark + "%M" + timemark + "%S";
	else
		fmt = "%Y" + datemark + "%m" + datemark + "%d";
	
	// invoke strptime()
	if ( strptime(datetime.c_str(),fmt.c_str(),&tm) != NULL )
		this->set( tm );
	else
		this->set();
}

/// 输出日期字符串
/// \param datemark 日期分隔字符,默认为"-"
/// \param leadingzero 是否补充前置零,默认为是
/// \return 输出指定格式的日期字符串
string DateTime::date( const string &datemark, const bool leadingzero ) const {
	char date_str[32];
	if ( leadingzero )
		snprintf( date_str, 32, "%04d%s%02d%s%02d", 
			this->year(), datemark.c_str(), this->month(), datemark.c_str(), this->m_day() );
	else
		snprintf( date_str, 32, "%d%s%d%s%d", 
			this->year(), datemark.c_str(), this->month(), datemark.c_str(), this->m_day() );
	
	return string( date_str );
}

/// 输出时间字符串
/// \param timemark 时间分隔字符,默认为":"
/// \param leadingzero 是否补充前置零,默认为是
/// \return 输出指定格式的时间字符串
string DateTime::time( const string &timemark, const bool leadingzero ) const {
	char time_str[32];
	if ( leadingzero )
		snprintf( time_str, 32, "%02d%s%02d%s%02d", 
			this->hour(), timemark.c_str(), this->min(), timemark.c_str(), this->sec() );
	else
		snprintf( time_str, 32, "%d%s%d%s%d", 
			this->hour(), timemark.c_str(), this->min(), timemark.c_str(), this->sec() );
	
	return string( time_str );
}

/// 输出日期时间字符串
/// \param datemark 日期分隔字符,默认为"-"
/// \param dtmark 日期时间分隔字符,默认为" "
/// \param timemark 时间分隔字符,默认为":"
/// \param leadingzero 是否补充前置零,默认为是
/// \return 输出指定格式的日期时间字符串
string DateTime::datetime( const string &datemark, const string &dtmark,
	const string &timemark, const bool leadingzero ) const 
{
	string datetime = this->date(datemark,leadingzero) + dtmark + 
		this->time(timemark,leadingzero);
	return datetime;
}

/// 输出 GMT 格式日期时间字符串
/// 主要用于设置 cookie 有效期
/// \return GMT 格式日期时间字符串
string DateTime::gmt_datetime() const {
	char gmt[50];
	struct tm gmt_tm;
	
	gmtime_r ( &_time, &gmt_tm );
	strftime( gmt, 50, "%A,%d-%B-%Y %H:%M:%S GMT", &gmt_tm );
	return string( gmt );
}

/// 赋值操作
DateTime& DateTime::operator=( const DateTime &date ) {
	if ( this == &date ) return *this;
	this->set( date );
	return *this;	
}
/// 赋值操作
DateTime& DateTime::operator=( const time_t &tt ) {
	this->set( tt );
	return *this;
}

/// 递增操作
DateTime& DateTime::operator+=( const DateTime &date ) {
	this->set( value() + date.value() );
	return *this;
}
/// 递增操作
DateTime& DateTime::operator+=( const time_t &tt ) {
	this->set( value() + tt );
	return *this;
}

/// 递减操作
DateTime& DateTime::operator-=( const DateTime &date ) {
	this->set( value() - date.value() );
	return *this;
}
/// 递减操作
DateTime& DateTime::operator-=( const time_t &tt ) {
	this->set( value() - tt );
	return *this;
}

/// 返回当月天数，范围1~31
int DateTime::m_days() const {
	int m = this->month();
	if ( m==1 || m==3 || m==5 || m==7 || m==8 || m==10 || m==12 ) { 
		return 31;
	} else if ( m == 2 ) {
		int leap = (this->year()) % 4;
		if ( leap == 0 ) {
			return 29;
		} else {
			return 28;
		}
	} else {
		return 30;
	}
}

/// 相加操作
DateTime operator+( const DateTime &date1, const DateTime &date2 ) {
	DateTime newdate;
	newdate.set( date1.value() + date2.value() );
	return newdate;
}
/// 相加操作
DateTime operator+( const DateTime &date, const time_t &tt ) {
	DateTime newdate;
	newdate.set( date.value() + tt );
	return newdate;
}

/// 相减操作
DateTime operator-( const DateTime &date1, const DateTime &date2 ) {
	DateTime newdate;
	newdate.set( date1.value() - date2.value() );
	return newdate;
}
/// 相减操作
DateTime operator-( const DateTime &date, const time_t &tt ) {
	DateTime newdate;
	newdate.set( date.value() - tt );
	return newdate;
}

} // namespace

