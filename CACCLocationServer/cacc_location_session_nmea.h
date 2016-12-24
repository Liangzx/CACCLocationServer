#ifndef _CACC_LOCATION_SESSION_NMEA_H_
#define _CACC_LOCATION_SESSION_NMEA_H_

#include "cacc_location_server_pub.h"
#include "cacc_location_server.h"
#include "cacc_location_session.h"

std::size_t const arry_size = 1024;
typedef std::array<char, arry_size> data_arry;
typedef boost::asio::streambuf stream_type;

class CACCLocationSessionNMEA : public CACCLocationSession,
	public std::enable_shared_from_this<CACCLocationSessionNMEA>
{
public:
	CACCLocationSessionNMEA(tcp::socket  socket);
	~CACCLocationSessionNMEA();
	virtual void init(std::shared_ptr<CACCLocationServer> ser_ptr);
	virtual void start();
private:
	void do_read();
	void do_write(std::size_t length);
	void do_wait();
	static std::string parse(std::string && str);
	data_arry data_;
	//stream_type data_;
	std::shared_ptr<CACCLocationServer> ser_ptr_;
	bool wait_flag_;
	tcp::socket socket_;
};
#endif // !_CACC_LOCATION_SESSION_NMEA_H_



