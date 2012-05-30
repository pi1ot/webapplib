/// \file waDateTime.h
/// webapp::DateTime类头文件
/// 日期时间运算

#ifndef _WEBAPPLIB_DATETIME_H_
#define _WEBAPPLIB_DATETIME_H_ 

#include <string>
#include <ctime>

using namespace std;

/// Web Application Library namaspace
namespace webapp {

/// \defgroup waDateTime waDateTime相关数据类型与全局函数

/// \ingroup waDateTime
/// \def TIME_ONE_SEC 
/// 时长定义一秒钟
#define	TIME_ONE_SEC	1

/// \ingroup waDateTime
/// \def TIME_ONE_MIN 
/// 时长定义一分钟
#define	TIME_ONE_MIN	60

/// \ingroup waDateTime
/// \def TIME_ONE_HOUR 
/// 时长定义一小时
#define	TIME_ONE_HOUR	3600

/// \ingroup waDateTime
/// \def TIME_ONE_DAY 
/// 时长定义一天
#define	TIME_ONE_DAY	86400

/// \ingroup waDateTime
/// \def TIME_ONE_WEEK 
/// 时长定义一周
#define	TIME_ONE_WEEK	604800

/// DateTime日期时间运算类
class DateTime {
	public:
	
	/// 默认构造函数,以当前时间构造对象
	DateTime() {
		this->set();
	}
	
	/// 参数为 time_t 的构造函数
	DateTime( const time_t &tt ) {
		this->set( tt );
	}
	
	/// 参数为指定时间的构造函数
	DateTime( const int year, const int mon, const int mday, 
		const int hour=0, const int min=0, const int sec=0 ) 
	{
		this->set( year, mon, mday, hour, min, sec );
	}
	
	/// 参数为 tm 结构的构造函数
	DateTime( const tm &st ) {
		this->set( st );
	}
	
	/// 参数为"YYYY-MM-DD HH:MM:SS"格式字符串的构造函数
	DateTime( const string &datetime, const string &datemark = "-", 
		const string &dtmark = " ", const string &timemark = ":" ) 
	{
		this->set( datetime, datemark, dtmark, timemark );
	}

	/// 拷贝构造函数
	DateTime ( const DateTime &date ) {
		this->set( date );
	}
		  
	/// 析构函数
	virtual ~DateTime() {}
	
	/// 赋值操作
	DateTime& operator=( const DateTime &date );
	/// 赋值操作
	DateTime& operator=( const time_t &tt );

	/// 递增操作
	DateTime& operator+=( const DateTime &date );
	/// 递增操作
	DateTime& operator+=( const time_t &tt );
	
	/// 递减操作
	DateTime& operator-=( const DateTime &date );
	/// 递减操作
	DateTime& operator-=( const time_t &tt );

	/// 返回四位数年份
	inline int year() const {return _tm.tm_year+1900;}
	/// 返回月份，范围1~12
	inline int month() const {return _tm.tm_mon+1;}
	/// 返回当月第几天，范围1~31
	inline int m_day() const {return _tm.tm_mday;}
	/// 返回当月天数，范围1~31
	int m_days() const;
	/// 返回当周第几天，周一至周六返回1~6，周日返回0
	inline int w_day() const {return _tm.tm_wday;}
	/// 返回当年第几天，范围0~365
	inline int y_day() const {return _tm.tm_yday;}
	/// 返回小时，范围0~23
	inline int hour() const {return _tm.tm_hour;}
	/// 返回分钟，范围0~59
	inline int min() const {return _tm.tm_min;}
	/// 返回秒数，范围0~59
	inline int sec() const {return _tm.tm_sec;}
	
	/// 返回 1970-1-1 0:0:0 以来的秒数
	inline long secs() const {return _time;}
	/// 返回 1970-1-1 0:0:0 以来的分钟数
	inline long mins() const {return ( _time/TIME_ONE_MIN );}
	/// 返回 1970-1-1 0:0:0 以来的小时数
	inline long hours() const {return ( _time/TIME_ONE_HOUR );}
	/// 返回 1970-1-1 0:0:0 以来的天数
	inline long days() const {return ( _time/TIME_ONE_DAY );}
	/// 返回 1970-1-1 0:0:0 以来的周数
	inline long weeks() const {return ( _time/TIME_ONE_WEEK );}
	
	/// 以当前时间设置对象
	void set();
	/// 以 time_t 参数设置对象
	void set( const time_t &tt );
	/// 以 tm 结构参数设置对象
	void set( const tm &st );
	/// 以指定时间设置对象
	void set( const int year, const int mon, const int mday, 
		const int hour=0, const int min=0, const int sec=0 );
	/// 以 DateTime 参数设置对象
	void set( const DateTime &date );
	/// 以"YYYY-MM-DD HH:MM:SS"格式字符串设置对象
	void set( const string &datetime, const string &datemark = "-", 
		const string &dtmark = " ", const string &timemark = ":" );
	
	/// 返回 time_t 类型的对象值
	inline time_t value() const {return this->secs();}
	/// 返回 struct tm 类型的对象值
	inline tm struct_tm() const {return _tm;}
	
	/// 输出日期字符串
	string date( const string &datemark = "-", 
		const bool leadingzero = true ) const;
				 
	/// 输出时间字符串
	string time( const string &timemark = ":", 
		const bool leadingzero = true ) const;
				 
	/// 输出日期时间字符串
	string datetime( const string &datemark = "-", const string &dtmark = " ",
		const string &timemark = ":", const bool leadingzero = true ) const;
					 
	/// 输出 GMT 格式日期时间字符串
	string gmt_datetime() const;
	
	////////////////////////////////////////////////////////////////////////////
	private:
	time_t _time;
	struct tm _tm;	
};

// operators

/// \ingroup waDateTime
/// \fn DateTime operator+( const DateTime &date1, const DateTime &date2 )
/// 时间相加
DateTime operator+( const DateTime &date1, const DateTime &date2 );

/// \ingroup waDateTime
/// \fn DateTime operator+( const DateTime &date, const time_t &tt )
/// 时间相加
DateTime operator+( const DateTime &date, const time_t &tt );

/// \ingroup waDateTime
/// \fn DateTime operator-( const DateTime &date1, const DateTime &date2 )
/// 时间相减
DateTime operator-( const DateTime &date1, const DateTime &date2 );

/// \ingroup waDateTime
/// \fn DateTime operator-( const DateTime &date, const time_t &tt )
/// 时间相减
DateTime operator-( const DateTime &date, const time_t &tt );

/// \ingroup waDateTime
/// \fn bool operator==( const DateTime &left, const DateTime &right )
/// 时间相等比较
inline bool operator==( const DateTime &left, const DateTime &right ) {
	return ( left.value() == right.value() );
}

/// \ingroup waDateTime
/// \fn bool operator==( const DateTime &left, const time_t &right )
/// 时间相等比较
inline bool operator==( const DateTime &left, const time_t &right ) {
	return ( left.value() == right );
}

/// \ingroup waDateTime
/// \fn bool operator!=( const DateTime &left, const DateTime &right )
/// 时间不相等比较
inline bool operator!=( const DateTime &left, const DateTime &right ) {
	return ( left.value() != right.value() );
}

/// \ingroup waDateTime
/// \fn bool operator!=( const DateTime &left, const time_t &right )
/// 时间不相等比较
inline bool operator!=( const DateTime &left, const time_t &right ) {
	return ( left.value() != right );
}

/// \ingroup waDateTime
/// \fn bool operator>( const DateTime &left, const DateTime &right )
/// 时间大于比较
inline bool operator>( const DateTime &left, const DateTime &right ) {
	return ( left.value() > right.value() );
}

/// \ingroup waDateTime
/// \fn bool operator>( const DateTime &left, const time_t &right )
/// 时间大于比较
inline bool operator>( const DateTime &left, const time_t &right ) {
	return ( left.value() > right );
}

/// \ingroup waDateTime
/// \fn bool operator<( const DateTime &left, const DateTime &right )
/// 时间小于比较
inline bool operator<( const DateTime &left, const DateTime &right ) {
	return ( left.value() < right.value() );
}

/// \ingroup waDateTime
/// \fn bool operator<( const DateTime &left, const time_t &right )
/// 时间小于比较
inline bool operator<( const DateTime &left, const time_t &right ) {
	return ( left.value() < right );
}

/// \ingroup waDateTime
/// \fn bool operator>=( const DateTime &left, const DateTime &right )
/// 时间不小于比较
inline bool operator>=( const DateTime &left, const DateTime &right ) {
	return ( left.value() >= right.value() );
}

/// \ingroup waDateTime
/// \fn bool operator>=( const DateTime &left, const time_t &right )
/// 时间不小于比较
inline bool operator>=( const DateTime &left, const time_t &right ) {
	return ( left.value() >= right );
}

/// \ingroup waDateTime
/// \fn bool operator<=( const DateTime &left, const DateTime &right )
/// 时间不大于比较
inline bool operator<=( const DateTime &left, const DateTime &right ) {
	return ( left.value() <= right.value() );
}

/// \ingroup waDateTime
/// \fn bool operator<=( const DateTime &left, const time_t &right ) 
/// 时间不大于比较
inline bool operator<=( const DateTime &left, const time_t &right ) {
	return ( left.value() <= right );
}

} // namespace

#endif //_WEBAPPLIB_DATETIME_H_

