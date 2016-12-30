#include <cstdlib>
#include <iostream>
#include <thread>
#include <queue>
#include "cacc_location_server_pub.h"
#include "cacc_location_server.h"
#include "cacc_location_config.h"
#include "spin_mutex.hpp"

//queue for ma
spin_mutex sm;
std::queue<std::string> que;

typedef std::chrono::time_point<std::chrono::high_resolution_clock> re_time_point;

void push_json_que(std::string json_str)
{
	std::lock_guard<spin_mutex> lock(sm);
	// TODO:push msg to rabbit mq
	que.emplace(json_str);
	std::cout << "mq push size:" << que.size() << std::endl;
}

//void pop_json_que(std::string & json_str)
//{
//	std::lock_guard<spin_mutex> lock(sm);
//	json_str = que.front();
//	que.pop();
//	std::cout << "mq pop size:" << que.size() << std::endl;
//}

void pkg_json_task(std::shared_ptr<CACCLocationServer> && server_ptr)
{
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
			std::string json_res = server_ptr->format(str);
			if (json_res.empty())
			{
				continue;
			}
			//std::cout << json_res << std::endl;
			//std::cout <<server_ptr->size() << std::endl;
			//TODO: push mq queue
			push_json_que(json_res);
		}
		else
		{
			std::this_thread::yield();
			//std::this_thread::sleep_for(std::chrono::seconds(1));
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