/*
Part of Ben Hoyt's INIH project
Function "atat" added by Andrew Eckel 2020-05-11

inih is released under the New BSD license, which is duplicated at the bottom of ini.h

Go to the project home page for more info:
https://github.com/benhoyt/inih
*/

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
	std::string atat(const std::string s) const;
};

#endif /* INIPARSER_H */
