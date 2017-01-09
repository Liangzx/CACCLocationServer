#include <cstdlib>
#include <iostream>
#include <thread>
#include <queue>
#include <cstddef>
#include "cacc_location_server_pub.h"
#include "cacc_location_server.h"
#include "cacc_location_config.h"
#include "spin_mutex.hpp"
#include "base_64.hpp"
#include "AMQPcpp.h"

//queue for ma
spin_mutex sm;
std::queue<std::string> que;
//

typedef std::chrono::time_point<std::chrono::high_resolution_clock> re_time_point;

//void push_json_que(std::string json_str)
//{
//	std::lock_guard<spin_mutex> lock(sm);
//	// TODO:push msg to rabbit mq
//	//que.emplace(json_str);
//	std::cout << "mq push size:" << que.size() << std::endl;
//}

//void pop_json_que(std::string & json_str)
//{
//	std::lock_guard<spin_mutex> lock(sm);
//	json_str = que.front();
//	que.pop();
//	std::cout << "mq pop size:" << que.size() << std::endl;
//}

std::string wrap_pkg(std::string const & buf, std::shared_ptr<CACCLocationServer> && server_ptr)
{
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	
	writer.String("appId");
	std::string app_id = "gis";
#if RAPIDJSON_HAS_STDSTRING
	writer.String(app_id);
#else
	writer.String(app_id.c_str(), static_cast<SizeType>(app_id.length()));
#endif

	writer.String("eventData");
#if RAPIDJSON_HAS_STDSTRING
	writer.String(buf);
#else
	writer.String(buf.c_str(), static_cast<SizeType>(buf.length()));
#endif

	writer.String("exchangeName");
	//std::string exchange_name = "exchange_gis";
	std::string exchange_name = server_ptr->config_->cfg_.mq_exchange_;
#if RAPIDJSON_HAS_STDSTRING
	writer.String(exchange_name);
#else
	writer.String(exchange_name.c_str(), static_cast<SizeType>(exchange_name.length()));
#endif

	writer.String("routeKey");
	//std::string route_key = "CTU_LOCGIS_Position_Data";
	std::string route_key = server_ptr->config_->cfg_.mq_route_key_;
#if RAPIDJSON_HAS_STDSTRING
	writer.String(route_key);
#else
	writer.String(route_key.c_str(), static_cast<SizeType>(route_key.length()));
#endif
	std::string wrp = sb.GetString();

	return wrp;
}

void push_json_que(std::string json_str)
{
	std::lock_guard<spin_mutex> lock(sm);
	// TODO:push msg to rabbit mq
	que.emplace(json_str);
	//std::cout << "mq push size:" << que.size() << std::endl;
}

void msg_to_mq(std::shared_ptr<CACCLocationServer> && server_ptr)
{

}

void pkg_json_task(std::shared_ptr<CACCLocationServer> && server_ptr)
{
	// connect amqp
	std::string conn = server_ptr->config_->cfg_.mq_constr_;
	std::string que_name = server_ptr->config_->cfg_.mq_queue_;
	std::string que_exchange = server_ptr->config_->cfg_.mq_exchange_;
	std::string que_type = server_ptr->config_->cfg_.mq_type_;
	std::string route_key = server_ptr->config_->cfg_.mq_route_key_;
	//string conn = "cls:111111@172.18.57.208:5673//acdm";
	AMQP amqp_produce;
	AMQPQueue * que;
	AMQPExchange * ex;
	try
	{
		std::cout << "send msg connect server begin..." << std::endl;
		amqp_produce.ConnectToServer(conn);
		//
		std::cout << "send msg connect server success..." << std::endl;
		ex = amqp_produce.createExchange(que_exchange);

		ex->Declare(que_exchange, que_type, AMQP_DURABLE);
		que = amqp_produce.createQueue(que_name);
		que->Declare(que_name, AMQP_DURABLE);
	
		que->Bind(que_exchange, route_key);
		ex->setHeader("Delivery-mode", 2);
		ex->setHeader("Content-type", "text/json");
		ex->setHeader("Content-encoding", "UTF-8");
	}
	catch (const AMQPException&e)
	{
		std::cout << e.getMessage() << std::endl;
		return;
	}
	base_64 b64;
	std::thread::id tid = std::this_thread::get_id();
	char base_en[4096] = { 0 };
	while (true)
	{
		msg m;
		if (!server_ptr->queue_empty() && server_ptr->pop(m))
		{
			// TODO: get packege from queue				
			std::shared_ptr<char> sh_ptr(m.msg_);
			char *ptr = m.msg_;
			std::string str(ptr, m.len_);
			// TODO: parse packege
			//std::string json_res = server_ptr->format(str);
			//--
			std::string buf = server_ptr->format(str);
			memset(base_en, 0, sizeof(base_en));
			b64.encode(base_en, buf.c_str(), buf.length());
			buf = base_en;
			rapidjson::StringBuffer sb;
			rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
			writer.StartObject();
			writer.String("appId");
			std::string app_id = "gis";
#if RAPIDJSON_HAS_STDSTRING
			writer.String(app_id);
#else
			writer.String(app_id.c_str(), static_cast<SizeType>(app_id.length()));
#endif

			writer.String("eventData");
#if RAPIDJSON_HAS_STDSTRING
			writer.String(buf);
#else
			writer.String(buf.c_str(), static_cast<SizeType>(buf.length()));
#endif
			writer.String("exchangeName");
			//std::string exchange_name = "exchange_gis";
			std::string exchange_name = server_ptr->config_->cfg_.mq_exchange_;
#if RAPIDJSON_HAS_STDSTRING
			writer.String(exchange_name);
#else
			writer.String(exchange_name.c_str(), static_cast<SizeType>(exchange_name.length()));
#endif

			writer.String("routeKey");
			//std::string route_key = "CTU_LOCGIS_Position_Data";
			std::string route_key = server_ptr->config_->cfg_.mq_route_key_;
#if RAPIDJSON_HAS_STDSTRING
			writer.String(route_key);
#else
			writer.String(route_key.c_str(), static_cast<SizeType>(route_key.length()));
#endif
			writer.EndObject();
			std::string json_res = sb.GetString();
			// TODO: wrap packeage
			if (json_res.empty())
			{
				continue;
			}
			//std::cout <<server_ptr->size() << std::endl;
			//TODO: push mq queue
			push_json_que(json_res);

			try
			{
				//std::lock_guard<spin_mutex> lck(sm);
				ex->Publish(json_res, route_key);
			}
			catch (const AMQPException&e)
			{
				//std::cout << e.getMessage() << std::endl;
				LOG4CPLUS_ERROR(server_ptr->logger_, e.getMessage().c_str());
			}
			
		}
		else
		{
			//std::this_thread::yield();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

//void array_json_mq()
//{
//	std::vector<std::string> mq_vec;
//	std::string buf;
//	re_time_point start = std::chrono::steady_clock::now();
//	re_time_point end = std::chrono::steady_clock::now();
//	using Ss = std::chrono::seconds;
//	StringBuffer sb;
//	PrettyWriter<StringBuffer> writer(sb);
//	///*Writer<StringBuffer> writer(sb);*/
//	// TODO: connect mq
//	while (true)
//	{
//		buf.clear();
//		pop_json_que(buf);
//		mq_vec.emplace_back(buf);
//		end = std::chrono::steady_clock::now();
//		if (std::chrono::duration_cast<Ss>((end-start)).count() >= 1)
//		{
//			std::cout << mq_vec.size() << std::endl;			
//			for (size_t i = 0; i < mq_vec.size(); i++)
//			{
//				writer.StartArray();
//				writer.String(mq_vec[i].c_str(), mq_vec[i].length());
//				writer.EndArray();
//			}
//			mq_vec.clear();
//			std::cout << sb.GetString() << std::endl;
//			sb.Flush();
//			start = std::chrono::steady_clock::now();
//		}
//	}
//}

int main(int argc, char* argv[])
{
	try
	{
		// TODO:parameters check
		if (argc != 2)
		{
			std::cout << "parameter number error!!!" << std::endl;
			std::cout << "usage:app protocol type[nema/app]";
			exit(-1);
		}		
		std::string packeage_type(argv[1]);

		if (!(boost::to_lower_copy(packeage_type) == "nema" 
			|| boost::to_lower_copy(packeage_type) == "app"))
		{
			std::cout << "usage:app protocol type[nema/app]";
			exit(-1);
		}
		// TODO:load config file
		// ÅäÖÃÎÄ¼þ
		std::shared_ptr<CACCLocationConfig> sh_cfg(new CACCLocationConfig(packeage_type));
		sh_cfg->cfg_.show();
		boost::asio::io_service io_service;
		tcp::endpoint endpoint(tcp::v4(), sh_cfg->cfg_.port_);
		const std::size_t que_buf = sh_cfg->cfg_.queue_size_;
		std::shared_ptr<CACCLocationServer> server_ptr(std::make_shared<CACCLocationServer>(
			io_service, endpoint, que_buf));
		server_ptr->set_config(std::move(sh_cfg));
		server_ptr->start();

		std::shared_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(io_service));
		std::thread io_server_th(std::bind(boost::bind(&boost::asio::io_service::run, &io_service)));

		// TODO: create thread to comuse msg from server's msg queue
		const int cpu_logic_cnt = std::thread::hardware_concurrency();
		std::size_t thread_cnt = cpu_logic_cnt;
		std::vector<std::thread> thread_vec;
		for (std::size_t i = 0; i < thread_cnt; i++)
		{
			thread_vec.push_back(std::thread(pkg_json_task, server_ptr));
		}

		// TODO: join threads
		for (std::size_t i = 0; i < thread_cnt; i++)
		{
			thread_vec[i].join();
		}
		io_server_th.join();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	
	return 0;
}