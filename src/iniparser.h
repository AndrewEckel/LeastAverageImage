/* Part of Ben Hoyt's INIH project https://github.com/benhoyt/inih */
/* Function "atat" added by Andrew Eckel 2020-05-11 */

#ifndef INIPARSER_H
#define INIPARSER_H

#include <ios>
#include <string>
#include <unordered_map>
#include <iostream>

#include "ini.h"

typedef std::unordered_map <std::string, std::string> ini_t;

class ini : public ini_t
{
public:
	// Constructors
	ini() = default;
	ini(const std::string &iniFile);
	// Member functions
	void parse(const std::string &iniFile);
	std::string atat(const std::string s);
};

#endif /* INIPARSER_H */
