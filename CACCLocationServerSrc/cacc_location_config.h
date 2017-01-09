#ifndef _CACC_LOCATION_CONFIG_H_
#define _CACC_LOCATION_CONFIG_H_

#include <unordered_map>
#include <fstream>
#include <iostream>
#include "cacc_location_server_pub.h"
#include "json/include/rapidjson/istreamwrapper.h"
#include "json/include/rapidjson/ostreamwrapper.h"
#include "json/include/rapidjson/prettywriter.h"

struct ConfigInfo
{
	// 缓冲队列满了是否等待
	bool if_wait_;
	// 缓冲队列的大小
	std::size_t queue_size_;
	// 程序名称
	std::string app_name_;
	// 连接类型
	std::string connect_type_;
	// mq 连接串
	std::string mq_constr_;
	// mq name
	std::string mq_queue_;
	// mq exchange
	std::string mq_exchange_;
	// mq type[direct/topic/fanout]
	std::string mq_type_;
	// mq route_key
	std::string mq_route_key_;
	// mq 
	// redis ip
	std::string redis_ip_;
	// redis port
	std::size_t redis_port_;
	// redis buf queue
	std::string redis_buf_queue_;
	// port 报文接收端口
	std::size_t port_;

	void show()
	{
		std::cout << if_wait_ << ":" << queue_size_ << ":" << ":"
			<< app_name_ << ":" << connect_type_ << ":" << mq_constr_ << ":"
			<< redis_ip_ << ":" << redis_port_  << ":" << redis_buf_queue_  << ":"
			<< port_ << std::endl;
	}
};

class CACCLocationConfig
{
public:
	CACCLocationConfig(std::string  packeage_type);
	~CACCLocationConfig();
	ConfigInfo cfg_;
	std::string packeage_type_;
private:
	bool load_config();
	bool get_cfg_bool_par(std::string const & par, bool defalut);
	void set_cfg_bool_par(std::string const & par, bool value);
	std::size_t get_cfg_int_par(std::string const & par, std::size_t defalut);
	std::string get_cfg_str_par(std::string const & par, std::string defalut);
	rapidjson::Document document_;
};
#endif // !_CACC_LOCATION_CONFIG_H_



