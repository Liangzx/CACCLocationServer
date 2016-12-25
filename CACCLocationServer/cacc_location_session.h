#ifndef _CACC_LOCATION_SESSION_H_
#define _CACC_LOCATION_SESSION_H_
#include <string>
#include <iostream>
#include <functional>
#include <unordered_map>
#include "cacc_location_server_pub.h"
#include "cacc_location_server.h"

class CACCLocationSession
{
public:
	CACCLocationSession();
	~CACCLocationSession();

	virtual void init(std::shared_ptr<CACCLocationServer>  ser_ptr) = 0;
	virtual void start() = 0;
};
#endif // !_CACC_LOCATION_SESSION_H_

