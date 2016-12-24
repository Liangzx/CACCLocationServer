#include "cacc_location_session_nmea.h"



CACCLocationSessionNMEA::CACCLocationSessionNMEA(tcp::socket socket):socket_(std::move(socket))
{
}

CACCLocationSessionNMEA::~CACCLocationSessionNMEA()
{
}

void CACCLocationSessionNMEA::init(std::shared_ptr<CACCLocationServer> ser_ptr)
{
	ser_ptr_ = ser_ptr;
	ser_ptr->parse = std::bind(&CACCLocationSessionNMEA::parse, std::placeholders::_1);
}

void CACCLocationSessionNMEA::start()
{
	do_read();
}

void CACCLocationSessionNMEA::do_read()
{
	//printf("CACCLocationSessionNMEA::do_read::begin\n");
	bool wait_flag = true;
	wait_flag_ = wait_flag;
	auto self(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(data_),
		[this, self](boost::system::error_code const & ec, std::size_t bytes_transferred) {
		if (!ec)
		{
			//printf("CACCLocationSessionNMEA::do_read::end\n");
			char *ptr = (char *)malloc(bytes_transferred);
			memcpy(ptr, data_.data(), bytes_transferred);
			int len = bytes_transferred;
			ser_ptr_->push(msg(len, ptr));
			printf("%d\n", ser_ptr_->size());
			if (wait_flag_)
			{
				do_wait();
			}
			do_write(bytes_transferred);
		}
	});
}

void CACCLocationSessionNMEA::do_write(std::size_t length)
{
	//printf("CACCLocationSessionNMEA::do_write::begin\n");
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

void CACCLocationSessionNMEA::do_wait()
{
	while (ser_ptr_->size() >= ser_ptr_->capacity())
	{
		//printf("CACCLocationSessionNMEA::do_wait\n");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

std::string CACCLocationSessionNMEA::parse(std::string && str)
{
	std::string buf = str;
	//std::cout << "CACCLocationSessionNMEA::parse--:" << buf << std::endl;
	return buf;
}

