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

// Constructor
ini::ini(const std::string &iniFile)
{
	parse(iniFile);
}
