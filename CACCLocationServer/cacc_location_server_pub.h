#ifndef _CACC_LOCATION_SERVER_PUB_H_
#define _CACC_LOCATION_SERVER_PUB_H_

#define _ENABLE_ATOMIC_ALIGNMENT_FIX
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>
#include <thread>

struct msg {
	msg(std::size_t len, char * m):len_(len),
		msg_(std::move(m)){}
	msg(){}
	std::size_t len_;
	char *msg_;
};

struct RecvPkg {
	double	latitude;
	double	longitude;
	float	altitude;
	float	north;
	float	east;
	float	height;
	int		satnum;
	int		posstat;
	float	dpop;
	float	hrms;
	float	vrms;
	float	age;
	float	speed;
	std::string time_stamp;
};

typedef boost::lockfree::queue<msg> lck_free_queue;
using boost::asio::ip::tcp;

#endif // !_CACC_LOCATION_SERVER_PUB_H_

