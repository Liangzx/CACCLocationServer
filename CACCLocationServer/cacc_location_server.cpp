#include "cacc_location_server.h"
#include "cacc_location_session_nmea.h"



CACCLocationServer::CACCLocationServer(boost::asio::io_service & io_service,
	tcp::endpoint const & endpoint, std::size_t que_buf)
	:acceptor_(io_service, endpoint),socket_(io_service),
	lck_free_queue_(que_buf),
	capacity_(que_buf)
{
	que_size_ = 0;
}


CACCLocationServer::~CACCLocationServer()
{
}

void CACCLocationServer::start(std::string packeage_type)
{
	packeage_type_ = std::move(packeage_type);
	do_accept();
}

void CACCLocationServer::do_accept()
{
	auto self(shared_from_this());
	acceptor_.async_accept(socket_, [this,self](boost::system::error_code ec) {
		if (!ec)
		{
			// TODO:
			std::shared_ptr<CACCLocationSession> session_ptr(std::make_shared<CACCLocationSessionNMEA>(std::move(socket_)));
			session_ptr->init(self);
			session_ptr->start();
		}
		do_accept();
	});
}
