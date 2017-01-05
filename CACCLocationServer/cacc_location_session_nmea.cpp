#include "cacc_location_session_nmea.h"


CACCLocationSessionNMEA::CACCLocationSessionNMEA(tcp::socket socket,
	CACCLocationServer * server): CACCLocationSession(std::move(socket), server)
{
	server->format = std::bind(&CACCLocationSessionNMEA::do_format, std::placeholders::_1);
}

CACCLocationSessionNMEA::~CACCLocationSessionNMEA()
{
}

void CACCLocationSessionNMEA::do_write(std::size_t length)
{
	do_read();
}

std::string CACCLocationSessionNMEA::do_format(std::string && str)
{
	std::string buf = str;
	std::size_t const field_cnt = 16;
	std::vector<std::string> split_vec;
	boost::split(split_vec, buf, boost::is_any_of(","), boost::token_compress_on);
	if (split_vec.size() <= field_cnt)
	{
		return std::string("");
	}

	RecvPkg pkg;
	//
	pkg.time_stamp_ = split_vec[1];
	pkg.latitude_ = boost::lexical_cast<double>(split_vec[2]);
	pkg.longitude_ = boost::lexical_cast<double>(split_vec[3]);
	pkg.altitude_ = boost::lexical_cast<double>(split_vec[4]);
	pkg.north_ = boost::lexical_cast<double>(split_vec[5]);
	pkg.east_ = boost::lexical_cast<double>(split_vec[6]);
	pkg.height_ = boost::lexical_cast<double>(split_vec[7]);
	pkg.satnum_ = boost::lexical_cast<int>(split_vec[8]);
	pkg.posstat_ = boost::lexical_cast<int>(split_vec[9]);
	pkg.dpop_ = boost::lexical_cast<double>(split_vec[10]);
	pkg.hrms_ = boost::lexical_cast<double>(split_vec[11]);
	pkg.vrms_ = boost::lexical_cast<double>(split_vec[12]);
	pkg.age_ = boost::lexical_cast<double>(split_vec[13]);
	pkg.speed_ = boost::lexical_cast<double>(split_vec[14]);
	pkg.direction_ = boost::lexical_cast<double>(split_vec[15]);
	pkg.object_id_ = split_vec[16];

	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	pkg.serialize(writer);

	buf = sb.GetString();
	return buf;
}

