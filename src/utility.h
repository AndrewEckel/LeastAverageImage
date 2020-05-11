#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <vector>
#include <stdexcept>

class Utility
{
public:
	//All functions are static.
	static std::string endWithSlash(std::string s);
	static bool stob(const std::string &str);
	static std::vector<std::string> splitByChars(std::string s, std::string split_chars);
	static std::vector<double> toDoubles(const std::vector<std::string> v, bool requireALLnumbers);
	static std::vector<int> toInts(const std::vector<std::string> v, bool requireALLints);
};

#endif //UTILITY_H
