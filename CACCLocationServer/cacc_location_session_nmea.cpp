#include "cacc_location_session_nmea.h"
#include "cacc_location_server.h"


CACCLocationSessionNMEA::CACCLocationSessionNMEA(tcp::socket socket,
	CACCLocationServer * server):socket_(std::move(socket))
{
	server->parse = std::bind(&CACCLocationSessionNMEA::parse, std::placeholders::_1);
}

CACCLocationSessionNMEA::~CACCLocationSessionNMEA()
{
}

void CACCLocationSessionNMEA::init(std::shared_ptr<CACCLocationServer>  ser_ptr)
{
	ser_ptr_ = std::move(ser_ptr);
}

void CACCLocationSessionNMEA::start()
{
	do_read();
}

void CACCLocationSessionNMEA::do_read()
{
	//printf("CACCLocationSessionNMEA::do_read::begin\n");
	bool wait_flag = true;
	wait_flag_ = wait_flag;
	auto self(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(data_),
		[this, self](boost::system::error_code const & ec, std::size_t bytes_transferred) {
		if (!ec)
		{
			//printf("CACCLocationSessionNMEA::do_read::end\n");
			char *ptr = (char *)malloc(bytes_transferred);
			memcpy(ptr, data_.data(), bytes_transferred);
			int len = bytes_transferred;
			ser_ptr_->push(msg(len, ptr));
			printf("%d\n", ser_ptr_->size());
			//if (connect_type_ == connect_type::short_connection)
			//{
			//	socket_.close();
			//}
			//if (wait_flag_)
			//{
			//	do_wait();
			//}
			do_write(bytes_transferred);
		}
	});
}

void CACCLocationSessionNMEA::do_write(std::size_t length)
{
	//printf("CACCLocationSessionNMEA::do_write::begin\n");
	auto self(shared_from_this());
	boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
		[this, self](boost::system::error_code const & ec, std::size_t /*bytes_transferred*/) {
		if (!ec)
		{
			//printf("CACCLocationSessionNMEA::do_write::end\n");
			do_read();
		}
	});
}

void CACCLocationSessionNMEA::do_wait()
{
	while (ser_ptr_->size() >= ser_ptr_->capacity())
	{
		//printf("CACCLocationSessionNMEA::do_wait\n");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

std::string CACCLocationSessionNMEA::parse(std::string && str)
{
	std::string buf = str;
	//std::cout << "CACCLocationSessionNMEA::parse--:" << buf << std::endl;
	std::vector<std::string> split_vec;
	//boost::replace_all(s, "@GELOC", "");
	//return std::string("@GELOC,2016-12-02T16:47:44.00,23.1666046368,113.4300508085,65.8410,0.0000,0.0000,0.0000,21,1,1.10,1.581,2.341,0,0.0083,91.0900,MG14341600009,,,*4A\r\n");
	boost::split(split_vec, buf, boost::is_any_of(","), boost::token_compress_on);
	
	if (split_vec.size() < 16)
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
	pkg.height_	= boost::lexical_cast<double>(split_vec[7]);
	pkg.satnum_	= boost::lexical_cast<int>(split_vec[8]);
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

