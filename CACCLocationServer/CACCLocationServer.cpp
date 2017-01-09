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

void push_json_que(std::string json_str)
{
	std::lock_guard<spin_mutex> lock(sm);
	// TODO:push msg to rabbit mq
	que.emplace(json_str);
}

void msg_to_mq(std::shared_ptr<CACCLocationServer> && server_ptr)
{
	// connect amqp
	std::string conn = server_ptr->config_->cfg_.mq_constr_;
	std::string que_name = server_ptr->config_->cfg_.mq_queue_;
	std::string que_exchange = server_ptr->config_->cfg_.mq_exchange_;
	std::string que_type = server_ptr->config_->cfg_.mq_type_;
	std::string route_key = server_ptr->config_->cfg_.mq_route_key_;

	AMQP amqp_produce;
	AMQPQueue * que_amq;
	AMQPExchange * ex;
	LOG4CPLUS_TRACE(server_ptr->logger_, "connect to mq server...");
	while (true)
	{
		//
		try
		{
			amqp_produce.ConnectToServer(conn);
			ex = amqp_produce.createExchange(que_exchange);
			ex->Declare(que_exchange, que_type, AMQP_DURABLE);
			que_amq = amqp_produce.createQueue(que_name);
			que_amq->Declare(que_name, AMQP_DURABLE);

			que_amq->Bind(que_exchange, route_key);
			ex->setHeader("Delivery-mode", 2);
			ex->setHeader("Content-type", "text/json");
			ex->setHeader("Content-encoding", "UTF-8");
		}
		catch (const AMQPException&e)
		{
			//std::cout << e.getMessage() << std::endl;
			LOG4CPLUS_FATAL(server_ptr->logger_, e.getMessage().c_str());
			std::this_thread::sleep_for(std::chrono::seconds(5));
			continue;
		}
		//
		std::unique_lock<spin_mutex> lock(sm);
		const std::size_t que_size = que.size();
		lock.unlock();
		for (size_t i = 0; i < que_size; i++)
		{
			std::string json_res = que.front();
			que.pop();
			try
			{
				ex->Publish(json_res, route_key);
			}
			catch (const AMQPException&e)
			{
				LOG4CPLUS_ERROR(server_ptr->logger_, e.getMessage().c_str());
				continue;
			}
			
		}		
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void pkg_json_task(std::shared_ptr<CACCLocationServer> && server_ptr)
{
	base_64 b64;
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
			std::string buf = server_ptr->format(str);
			LOG4CPLUS_INFO(server_ptr->logger_, ("[PKG]" + buf).c_str());
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
			std::string exchange_name = server_ptr->config_->cfg_.mq_exchange_;
#if RAPIDJSON_HAS_STDSTRING
			writer.String(exchange_name);
#else
			writer.String(exchange_name.c_str(), static_cast<SizeType>(exchange_name.length()));
#endif

			writer.String("routeKey");
			std::string route_key = server_ptr->config_->cfg_.mq_route_key_;
#if RAPIDJSON_HAS_STDSTRING
			writer.String(route_key);
#else
			writer.String(route_key.c_str(), static_cast<SizeType>(route_key.length()));
#endif
			writer.EndObject();
			std::string json_res = sb.GetString();

			if (json_res.empty())
			{
				continue;
			}
			//TODO: push mq queue
			push_json_que(json_res);
			LOG4CPLUS_INFO(server_ptr->logger_, ("[DES]" + json_res).c_str());
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}


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
		std::size_t thread_cnt = cpu_logic_cnt - 2 > 0 ? cpu_logic_cnt - 2 : 2;
		std::vector<std::thread> thread_vec;
		//
		for (std::size_t i = 0; i < thread_cnt; i++)
		{
			thread_vec.push_back(std::thread(pkg_json_task, server_ptr));
		}
		// push mq thread
		std::thread push_mq_th(msg_to_mq, server_ptr);
		// TODO: join threads
		for (std::size_t i = 0; i < thread_cnt; i++)
		{
			thread_vec[i].join();
		}
		io_server_th.join();
		push_mq_th.join();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	
	return 0;
}