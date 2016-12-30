#include "cacc_location_config.h"

CACCLocationConfig::CACCLocationConfig(std::string packeage_type)
{
	packeage_type_ = packeage_type;
	load_config();
}

CACCLocationConfig::~CACCLocationConfig()
{
}

bool CACCLocationConfig::load_config()
{
	try
	{
		char const * path = "C:/Users/able_/Desktop/CACCLocationServer/CACCLocationServer/CACCLocationServer/CACCLocationServer.json";

		std::ifstream ifs(path);
		IStreamWrapper isw(ifs);

		document_.ParseStream(isw);

		// ���ݴ����Э���ȡ��Ӧ������
		assert(document_.HasMember(packeage_type_.c_str()));
		assert(document_[packeage_type_.c_str()].IsObject());

		cfg_.if_wait_ = get_cfg_bool_par("if_wait", false);
		cfg_.queue_size_ = get_cfg_int_par("queue_size", 4096);
		cfg_.app_name_ = get_cfg_str_par("app_name", "CACCLocationServer");
		cfg_.connect_type_ = get_cfg_str_par("connect_type", "persistent_connection");
		cfg_.mq_constr_ = get_cfg_str_par("mq_constr", "");
		assert(cfg_.mq_constr_ != "");
		cfg_.mq_queue_ = get_cfg_str_par("mq_queue", "");
		assert(cfg_.mq_queue_ != "");
		cfg_.redis_ip_ = get_cfg_str_par("redis_ip", "");
		assert(cfg_.redis_ip_ != "");
		cfg_.redis_port_ = get_cfg_int_par("redis_port", 12345);
		cfg_.redis_buf_queue_ = get_cfg_str_par("redis_buf_queue", "");
		assert(cfg_.redis_buf_queue_ != "");
		cfg_.port_ = get_cfg_int_par("port", 12277);
		
		return true;
	}
	catch (const std::exception & e)
	{
		printf_s("%s:%s\n", __FUNCTION__,e.what());
		return false;
	}	
}

bool CACCLocationConfig::get_cfg_bool_par(std::string const & par, bool defalut)
{
	std::string cfg_buf = "/" + packeage_type_ + "/" + par;
	return Pointer(cfg_buf.c_str()).GetWithDefault(document_, defalut).GetBool();
}

std::size_t CACCLocationConfig::get_cfg_int_par(std::string const & par, std::size_t defalut)
{
	std::string cfg_buf = "/" + packeage_type_ + "/" + par;
	return Pointer(cfg_buf.c_str()).GetWithDefault(document_, defalut).GetInt();
}

std::string CACCLocationConfig::get_cfg_str_par(std::string const & par, std::string  defalut)
{
	std::string cfg_buf = "/" + packeage_type_ + "/" + par;
	return Pointer(cfg_buf.c_str()).GetWithDefault(document_, defalut.c_str()).GetString();
}