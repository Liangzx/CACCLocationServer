#include "cacc_location_server.h"
#include "cacc_location_session_nmea.h"
#include "cacc_location_session_app.h"


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

void CACCLocationServer::start()
{
	do_accept();
}

void CACCLocationServer::do_accept()
{
	auto self(shared_from_this());
	acceptor_.async_accept(socket_, [this,self](boost::system::error_code ec) {
		if (!ec)
		{
			// TODO:
			std::shared_ptr<CACCLocationSession> session_ptr = nullptr;
			if (config_->packeage_type_ == boost::to_lower_copy(std::string("nema")))
			{
				session_ptr = (std::make_shared<CACCLocationSessionNMEA>(std::move(socket_),
					this));
			}
			else if(config_->packeage_type_ == boost::to_lower_copy(std::string("app")))
			{
				session_ptr = (std::make_shared<CACCLocationSessionApp>(std::move(socket_),
					this));
			}
			assert(session_ptr != nullptr);
			session_ptr->init(std::move(self));
			session_ptr->start();
		}
		do_accept();
	});
}

void CACCLocationServer::set_config(std::shared_ptr<CACCLocationConfig> && cfg)
{
	this->config_ = std::move(cfg);
}
