////#include <iostream>
////#include <vector>
////#include <string>
////#include "boost/algorithm/string.hpp"
////
////int main(int argc, char ** argv)
////{
////
////	std::string s = "@GELOC,2016-12-02T16:47:44.00,23.1666046368,113.4300508085,65.8410,0.0000,0.0000,0.0000,21,1,1.10,1.581,2.341,0,0.0083,91.0900,MG14341600009,,,*4A";
////	boost::to_upper(s);
////	std::vector<std::string> split_vec;
////	boost::replace_all(s, "@GELOC", "");
////	boost::split(split_vec, s, boost::is_any_of(","), boost::token_compress_on);
////
////	std::cout << split_vec[0] << std::endl;
////	
////	
////	return 0;
////}