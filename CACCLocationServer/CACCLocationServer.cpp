#include <cstdlib>
#include <iostream>
#include <thread>
#include "cacc_location_server.h"
#include "cacc_location_server_pub.h"


void task(std::shared_ptr<CACCLocationServer> && server_ptr)
{
	while (true)
	{
		msg m;
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		if (!server_ptr->queue_empty() && server_ptr->pop(m))
		{
			// TODO: get packege from queue				
			std::shared_ptr<char> sh_ptr(m.msg_);
			char *ptr = m.msg_;
			std::string str(ptr, m.len_);
			//std::cout << "servers.pop:" + str << std::endl;
			// TODO: parse packege
			std::string json_res = server_ptr->parse(str);
			//std::cout << json_res << std::endl;
			std::cout << server_ptr->size() << std::endl;
			// TODO: push mq
		}
		//else
		//{
		//	std::this_thread::sleep_for(std::chrono::milliseconds(1));
		//	std::cout << "empty:queue::sleep_for(std::chrono::seconds(5))" << std::endl;
		//}
	}
}


int main(int argc, char* argv[])
{
	try
	{
		// TODO:load config file
		boost::asio::io_service io_service;
		tcp::endpoint endpoint(tcp::v4(), 10086);
		std::string packeage_type = "CACCLocationSessionNMEA";

		const std::size_t que_buf = 1000;
		std::shared_ptr<CACCLocationServer> server_ptr(std::make_shared<CACCLocationServer>(
			io_service, endpoint, que_buf));
		server_ptr->start(packeage_type);

		std::shared_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(io_service));
		std::thread t(std::bind(boost::bind(&boost::asio::io_service::run, &io_service)));

		// TODO: create thread to comuse msg from server's msg queue
		const int cpu_logic_cnt = std::thread::hardware_concurrency();
		std::size_t thread_cnt = cpu_logic_cnt / 2;
		std::vector<std::thread> thread_vec;
		for (std::size_t i = 0; i < thread_cnt; i++)
		{
			thread_vec.push_back(std::thread(task, server_ptr));
		}

		// TODO: join threads
		for (std::size_t i = 0; i < thread_cnt; i++)
		{
			thread_vec[i].join();
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	
	return 0;
}