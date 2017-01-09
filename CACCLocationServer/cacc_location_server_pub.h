#ifndef _CACC_LOCATION_SERVER_PUB_H_
#define _CACC_LOCATION_SERVER_PUB_H_

#define _ENABLE_ATOMIC_ALIGNMENT_FIX
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>
#include <thread>
#include <string>
#include "json/include/rapidjson/prettywriter.h" // for stringify JSON
#include "json/include/rapidjson/document.h"
#include "json/include/rapidjson/pointer.h"
#include "cacc_location_config.h"
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/loggingmacros.h>

using namespace std;
using namespace rapidjson;
using boost::asio::ip::tcp;

#include <cstddef>

struct msg {
	msg(std::size_t len, char * m):len_(len),
		msg_(std::move(m)){}
	msg(){}
	std::size_t len_;
	char *msg_;
};

struct RecvPkg {
	int		satnum_;
	int		posstat_;				// 解状态
	int		rssi_;					// 信号强度
	int		lac_;					// 小区Lac
	int		ci_;					// 小区Cell
	double	latitude_;				// 纬度
	double	longitude_;				// 经度
	double	altitude_;				// 海拔
	double	north_;					// 北坐标
	double	east_;					// 东坐标
	double	height_;				// 高程
	double  local_x_;				// 相对基站位置x
	double	local_y_;				// 相对基站位置y
	double	dpop_;
	double	hrms_;
	double	vrms_;
	double	age_;					// 差分延时
	double	speed_;					// 速度
	double	direction_;				// 方向
	std::string time_stamp_;		// 报文时间戳
	std::string object_id_;			// id 车载设备/app
	std::size_t obj_type_;

	void clear()
	{
		latitude_ = 0.0;
		longitude_ = 0.0;
		altitude_ = 0.0;
		north_ = 0.0;
		east_ = 0.0;
		height_ = 0.0;
		satnum_ = 0;
		posstat_ = 0;
		dpop_ = 0.0;
		hrms_ = 0.0;
		vrms_ = 0.0;
		age_ = 0.0;
		speed_ = 0.0;
		direction_ = 0.0;
		time_stamp_ = "";
		object_id_ = "";
		rssi_ = 0;
		lac_ = 0;
		ci_ = 0;
		obj_type_ = 0;
	}

	template<typename Writer>
	void serialize(Writer & writer)const
	{
		writer.StartObject();

		//writer.String("object_id");
		writer.String("userId");
#if RAPIDJSON_HAS_STDSTRING
		writer.String(object_id_);
#else
		writer.String(object_id_.c_str(), static_cast<SizeType>(object_id_.length()));
#endif
		//writer.String("time_stamp");
		writer.String("objType");
		writer.Int(obj_type_);
		writer.String("recordTime");
#if RAPIDJSON_HAS_STDSTRING
		writer.String(time_stamp_);
#else
		writer.String(time_stamp_.c_str(), static_cast<SizeType>(time_stamp_.length()));
#endif
		writer.String("latitude");
		writer.Double(latitude_);
		writer.String("longitude");
		writer.Double(longitude_);
		//writer.String("altitude");
		writer.String("elevation");
		writer.Double(altitude_);
		//writer.String("north");
		//writer.Double(north_);
		//writer.String("east");
		//writer.Double(east_);
		//writer.String("height");
		//writer.Double(height_);
		//writer.String("satnum");
		//writer.Int(satnum_);
		//writer.String("posstat");
		//writer.Int(posstat_);
		//writer.String("dpop");
		//writer.Double(dpop_);
		//writer.String("hrms");
		//writer.Double(hrms_);
		//writer.String("vrms");
		//writer.Double(vrms_);
		//writer.String("age");
		//writer.Double(age_);
		writer.String("speed");
		writer.Double(speed_);
		writer.String("diretion");
		writer.Double(direction_);

		writer.EndObject();
	}
};

typedef boost::lockfree::queue<msg> lck_free_queue;
std::size_t const arry_size = 1024;
typedef std::array<char, arry_size> data_arry;
typedef boost::asio::streambuf stream_type;

#endif // !_CACC_LOCATION_SERVER_PUB_H_

