#ifndef _CACC_LOCATION_SESSION_H_
#define _CACC_LOCATION_SESSION_H_
#include <string>
#include <iostream>
#include <functional>
#include <unordered_map>
#include "cacc_location_server_pub.h"
#include "cacc_location_server.h"

class CACCLocationSession
	:public std::enable_shared_from_this<CACCLocationSession>
{
public:
	CACCLocationSession(tcp::socket  socket, CACCLocationServer * server);
	~CACCLocationSession();
	void init(std::shared_ptr<CACCLocationServer>  ser_ptr);
	void start();
	virtual void do_read();
	virtual void do_write(std::size_t length);
	virtual void do_wait();
protected:
	std::string until_char_;
	data_arry data_;
	stream_type data_s_;
	std::shared_ptr<CACCLocationServer> ser_ptr_;
	// 队列满了是否等待
	bool wait_flag_;
	tcp::socket socket_;
	// 长/短连接
	enum connect_type
	{
		persistent_connection,
		short_connection
	}connect_type_;
private:
	static std::string do_format(std::string && str);
};
#endif // !_CACC_LOCATION_SESSION_H_

