/* Part of Ben Hoyt's INIH project https://github.com/benhoyt/inih */

#include "iniparser.h"

static int handler(void *config, const char *section, const char *name, const char *value)
{
	// Concatenate section + name to make key
	std::string key = std::string(section) + "_" + std::string(name);
	// Insert or update key-value pair in options map
	ini_t *opts = (ini_t *)config;
	(*opts)[key] = value;
	return 0;
}

void ini::parse(const std::string &iniFile)
{
	if(ini_parse(iniFile.c_str(), handler, this) < 0)
	{
		throw std::ios::failure("ini::parse: Could not read '" + iniFile + "'");
	}
}

std::string ini::atat(const std::string s) const
{
	//Identical to the "at" function, except that if there is an error, it prints a nice error message before throwing it.
	try{
		return at(s);
	} catch(std::exception e){
		std::cerr << "INI Parser: Error finding entry \"" << s << "\"" << std::endl;
		throw e;
	}
}

// Constructor
ini::ini(const std::string &iniFile)
{
	parse(iniFile);
}
