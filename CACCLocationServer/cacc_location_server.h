#ifndef _CACC_LOCATION_SERVER_H_
#define _CACC_LOCATION_SERVER_H_

#include "cacc_location_server_pub.h"

using boost::asio::ip::tcp;

class CACCLocationServer 
	:public std::enable_shared_from_this<CACCLocationServer>
{
public:
	CACCLocationServer(boost::asio::io_service & io_service,
		tcp::endpoint const & endpoint, std::size_t que_buf);
	~CACCLocationServer();
	void start();
	bool pop(msg & m)
	{
		if (lck_free_queue_.pop(m))
		{			
			--que_size_;
			return true;
		}
		return false;
	}
	bool push(const msg & m)
	{
		if (lck_free_queue_.push(m))
		{
			++que_size_;
			return true;
		}
		return false;
	}
	bool queue_empty()
	{
		return lck_free_queue_.empty();
	}
	bool is_lck_fre()
	{
		return lck_free_queue_.is_lock_free();
	}
	const std::size_t size()
	{
		return que_size_;
	}
	const std::size_t capacity() const
	{
		return capacity_;
	}

	std::function<std::string(std::string)> format;
	std::shared_ptr<CACCLocationConfig> config_;
	void set_config(std::shared_ptr<CACCLocationConfig> && cfg);
private:
	void do_accept();	
	tcp::socket socket_;
	tcp::acceptor acceptor_;
	lck_free_queue lck_free_queue_;
	std::atomic<std::size_t> que_size_;
	std::size_t capacity_;
	std::string packeage_type_;
};

#endif // !_CACC_LOCATION_SERVER_H_


