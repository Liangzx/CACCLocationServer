#ifndef _CACC_LOCATION_SESSION_NMEA_H_
#define _CACC_LOCATION_SESSION_NMEA_H_

#include "cacc_location_server_pub.h"
#include "cacc_location_server.h"
#include "cacc_location_session.h"



class CACCLocationSessionNMEA : public CACCLocationSession
{
public:
	CACCLocationSessionNMEA(tcp::socket  socket, CACCLocationServer * server);
	~CACCLocationSessionNMEA();

private:
	static std::string do_format(std::string && str);
};
#endif // !_CACC_LOCATION_SESSION_NMEA_H_



