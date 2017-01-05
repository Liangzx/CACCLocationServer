#include <algorithm>
#include "cacc_location_session.h"

CACCLocationSession::CACCLocationSession(tcp::socket socket, CACCLocationServer * server)
	:socket_(std::move(socket))
{
	until_char_ = "\r\n";
	connect_type_ = connect_type::persistent_connection;
	if (boost::to_lower_copy(server->config_->cfg_.connect_type_) == "short_connection")
	{
		connect_type_ = connect_type::short_connection;
	}
	server->format = std::bind(&CACCLocationSession::do_format, std::placeholders::_1);
}

CACCLocationSession::~CACCLocationSession()
{
}

void CACCLocationSession::init(std::shared_ptr<CACCLocationServer> ser_ptr)
{
	ser_ptr_ = std::move(ser_ptr);
}

void CACCLocationSession::start()
{
	do_read();
}

void CACCLocationSession::do_read()
{
	try
	{
		//@GELOC,2016-12-02T16:47:44.00,23.1666046368,113.4300508085,65.8410,0.0000,0.0000,0.0000,21,1,1.10,1.581,2.341,0,0.0083,91.0900,MG14341600009,,,*4A\r\n
		//printf("CACCLocationSessionNMEA::do_read::begin\n");
		wait_flag_ = ser_ptr_->config_->cfg_.if_wait_;
		auto self(shared_from_this());
		boost::asio::async_read_until(socket_, data_s_, until_char_,
			[this, self](boost::system::error_code const & ec, std::size_t bytes_transferred) {
			if (!ec)
			{
				std::istream is(&data_s_);
				std::string line;
				std::getline(is, line);

				char *ptr = (char *)malloc(line.length());
				memcpy(ptr, line.c_str(), line.length());
				ser_ptr_->push(msg(line.length(), ptr));
				if (connect_type_ == connect_type::short_connection)
				{
					socket_.close();
				}
				if (wait_flag_)
				{
					do_wait();
				}
				do_write(bytes_transferred);
			}
		});
	}
	catch (const std::exception&e)
	{
		printf("CACCLocationSessionNMEA::do_read::%s\n", e.what());
	}
}

void CACCLocationSession::do_write(std::size_t length)
{
	auto self(shared_from_this());
	boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
		[this, self](boost::system::error_code const & ec, std::size_t /*bytes_transferred*/) {
		if (!ec)
		{
			//printf("CACCLocationSessionNMEA::do_write::end\n");
			do_read();
		}
	});
}

void CACCLocationSession::do_wait()
{
	while (ser_ptr_->size() >= ser_ptr_->capacity())
	{
		//printf("CACCLocationSessionNMEA::do_wait\n");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}



std::string CACCLocationSession::do_format(std::string && str)
{
	std::cout << "CACCLocationSession::do_format" << std::endl;
	return std::string("");
}
