#ifndef _CACC_LOCATION_SESSION_APP_H_
#define _CACC_LOCATION_SESSION_APP_H_
#include "cacc_location_server_pub.h"
#include "cacc_location_server.h"
#include "cacc_location_session.h"

class CACCLocationSessionApp
	: public CACCLocationSession
{
public:
	CACCLocationSessionApp(tcp::socket  socket, CACCLocationServer * server);
	~CACCLocationSessionApp();
	virtual void do_write(std::size_t length);
private:
	static std::string do_format(std::string && str);
};
#endif // !_CACC_LOCATION_SESSION_APP_H_


