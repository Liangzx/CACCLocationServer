#include <cstdlib>
#include <iostream>
#include <thread>
#include "cacc_location_server.h"
#include "cacc_location_server_pub.h"
#include "json/include/rapidjson/rapidjson.h"
#include "json/include/rapidjson/reader.h"
#include "json/include/rapidjson/writer.h"

using namespace std;
using namespace rapidjson;

void task(std::shared_ptr<CACCLocationServer> && server_ptr)
{
	while (true)
	{
		msg m;
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		if (server_ptr->pop(m) || !server_ptr->queue_empty())
		{
			// TODO: get packege from queue				
			std::shared_ptr<char> sh_ptr(m.msg_);
			std::string str(sh_ptr.get(), m.len_);
			//std::cout << "servers.pop:" + str << std::endl;
			// TODO: parse packege
			server_ptr->parse(str);
			// TODO: push mq
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::seconds(5));
			//std::cout << "empty:queue::sleep_for(std::chrono::seconds(5))" << std::endl;
		}
	}
}

struct MyHandler {

	bool Null() { cout << "Null()" << endl; return true; }

	bool Bool(bool b) { cout << "Bool(" << boolalpha << b << ")" << endl; return true; }

	bool Int(int i) { cout << "Int(" << i << ")" << endl; return true; }

	bool Uint(unsigned u) { cout << "Uint(" << u << ")" << endl; return true; }

	bool Int64(int64_t i) { cout << "Int64(" << i << ")" << endl; return true; }

	bool Uint64(uint64_t u) { cout << "Uint64(" << u << ")" << endl; return true; }

	bool Double(double d) { cout << "Double(" << d << ")" << endl; return true; }

	bool RawNumber(const char* str, SizeType length, bool copy) {

		cout << "Number(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;

		return true;

	}

	bool String(const char* str, SizeType length, bool copy) {

		cout << "String(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;

		return true;

	}

	bool StartObject() { cout << "StartObject()" << endl; return true; }

	bool Key(const char* str, SizeType length, bool copy) {

		cout << "Key(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;

		return true;

	}

	bool EndObject(SizeType memberCount) { cout << "EndObject(" << memberCount << ")" << endl; return true; }

	bool StartArray() { cout << "StartArray()" << endl; return true; }

	bool EndArray(SizeType elementCount) { cout << "EndArray(" << elementCount << ")" << endl; return true; }

};



//int main(int argc, char* argv[])
//{
//	////try
//	////{
//	////	// TODO:load config file
//	////	boost::asio::io_service io_service;
//	////	tcp::endpoint endpoint(tcp::v4(), 10086);
//	////	std::string packeage_type = "CACCLocationSessionNMEA";
//
//	////	const std::size_t que_buf = 128;
//	////	std::shared_ptr<CACCLocationServer> server_ptr(std::make_shared<CACCLocationServer>(
//	////		io_service, endpoint, que_buf));
//	////	server_ptr->start(packeage_type);
//
//	////	std::shared_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(io_service));
//	////	std::thread t(std::bind(boost::bind(&boost::asio::io_service::run, &io_service)));
//
//	////	// TODO: create thread to comuse msg from server's msg queue
//	////	const int cpu_logic_cnt = std::thread::hardware_concurrency();
//	////	std::size_t thread_cnt = cpu_logic_cnt / 2;
//	////	std::vector<std::thread> thread_vec;
//	////	for (std::size_t i = 0; i < thread_cnt; i++)
//	////	{
//	////		thread_vec.push_back(std::thread(task, server_ptr));
//	////	}
//
//	////	// TODO: join threads
//	////	for (std::size_t i = 0; i < thread_cnt; i++)
//	////	{
//	////		thread_vec[i].join();
//	////	}
//	////}
//	////catch (std::exception& e)
//	////{
//	////	std::cerr << "Exception: " << e.what() << "\n";
//	////}
//	const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4]";
//	rapidjson::Reader reader;
//	rapidjson::StringStream ss(json);
//	MyHandler handler;
//	reader.Parse(ss, handler);
//	return 0;
//}