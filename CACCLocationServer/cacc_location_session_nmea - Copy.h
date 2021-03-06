#ifndef _CACC_LOCATION_SESSION_NMEA_H_
#define _CACC_LOCATION_SESSION_NMEA_H_

#include "cacc_location_server_pub.h"
#include "cacc_location_server.h"
#include "cacc_location_session.h"



class CACCLocationSessionNMEA : public CACCLocationSession/*,
	public std::enable_shared_from_this<CACCLocationSessionNMEA>*/
{
public:
	CACCLocationSessionNMEA(tcp::socket  socket, CACCLocationServer * server);
	~CACCLocationSessionNMEA();
	virtual void init(std::shared_ptr<CACCLocationServer>  ser_ptr);
	virtual void start();
	void do_parse(std::size_t const bytes_transferred);

private:
	void do_read();
	void do_write(std::size_t length);
	void do_wait();
	static std::string do_format(std::string && str);
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
};
#endif // !_CACC_LOCATION_SESSION_NMEA_H_



