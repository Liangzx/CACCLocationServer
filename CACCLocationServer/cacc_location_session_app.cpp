#include "cacc_location_session_app.h"

CACCLocationSessionApp::CACCLocationSessionApp(tcp::socket socket,
	CACCLocationServer * server) : CACCLocationSession(std::move(socket), server)
{
	server->format = std::bind(&CACCLocationSessionApp::do_format, std::placeholders::_1);
}

CACCLocationSessionApp::~CACCLocationSessionApp()
{
}

void CACCLocationSessionApp::do_write(std::size_t length)
{
	auto self(shared_from_this());
	// TODO:ask client
	const char * buf = "OK";
	boost::asio::async_write(socket_, boost::asio::buffer(buf, 3),
		[this, self](boost::system::error_code const & ec, std::size_t /*bytes_transferred*/) {
		if (!ec)
		{
			//printf("CACCLocationSessionNMEA::do_write::end\n");
			do_read();
		}
	});
}

std::string CACCLocationSessionApp::do_format(std::string && str)
{
	std::string buf = str;
	std::size_t const field_cnt = 11;
	std::vector<std::string> split_vec;
	boost::split(split_vec, buf, boost::is_any_of(","), boost::token_compress_on);

	if (split_vec.size() <= field_cnt)
	{
		return std::string("");
	}

	RecvPkg pkg;
	pkg.clear();
	//
	try
	{
		pkg.time_stamp_ = split_vec[1];
		pkg.latitude_ = boost::lexical_cast<double>(split_vec[2]);
		pkg.longitude_ = boost::lexical_cast<double>(split_vec[3]);
		pkg.altitude_ = boost::lexical_cast<double>(split_vec[4]);
		pkg.object_id_ = split_vec[5];
		pkg.rssi_ = boost::lexical_cast<int>(split_vec[6]);
		pkg.lac_ = boost::lexical_cast<int>(split_vec[7]);
		pkg.ci_ = boost::lexical_cast<int>(split_vec[8]);
		pkg.local_x_ = boost::lexical_cast<double>(split_vec[9]);
		pkg.local_y_ = boost::lexical_cast<double>(split_vec[10]);
		pkg.direction_ = boost::lexical_cast<double>(split_vec[11]);
	}
	catch (const std::exception& e)
	{
		printf("CACCLocationSessionApp::do_format:%s\n", e.what());
		return std::string("");
	}
	

	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	pkg.serialize(writer);

	buf = sb.GetString();

	return buf;
}
