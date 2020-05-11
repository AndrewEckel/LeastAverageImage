#include "utility.h"

std::string Utility::endWithSlash(std::string s) {
	//If s ends with a slash, returns s.
	//Otherwise, returns s + a slash.
	if (s.at(s.length() - 1) == '/' || s.at(s.length() - 1) == '\\') {
		return s;
	}
	else return (s + "/");
}

// Convert string to boolean
bool Utility::stob(const std::string &str)
{
	if (str == "true")
	{
		return true;
	}
	else if (str != "false")
	{
		throw std::invalid_argument("stob: expected boolean to be 'true' or 'false', but got '" + str + "' instead");
	}
	return false;
}

std::vector<std::string> Utility::splitByChars(std::string s, std::string split_chars)
{
	//returns a vector of every substring of s that does not contain any of the characters in split_chars (guaranteed to be in order)
	size_t n = s.find_last_of(split_chars);

	if (n == std::string::npos) {  //no split chars found: 
		std::vector<std::string> r;
		if (s.size() == 0) {//s is empty: return empty vector (base case 1)
			return r;
		}
		else { //s contains non-split characters only: return a vector containing only s (base case 2)
			r.push_back(s);
			return r;
		}
	}
	else if (n == s.size() - 1) {  //last character is a split char: return splitByChars(rest of string)
		return splitByChars(s.substr(0, s.size() - 1), split_chars);
	}
	else {  //any other character is last split char: get splitByChars(rest of string), add the post-split substring to it, and return.
		std::vector<std::string> r = splitByChars(s.substr(0, n), split_chars);
		r.push_back(s.substr(n + 1, s.size()));
		return r;
	}
}

std::vector<double> Utility::toDoubles(const std::vector<std::string> v, bool requireALLnumbers)
{
	//Input: A vector of any size, of strings that each represent a value that can be expressed as a double.
	//Output: A vector of those doubles, in the same order as the original strings.

	//If requireALLnumbers is set to false, then the returned vector will consist of only the entries that were interpretable as doubles.
	//So for example, this vector of four strings { "3.0", "4", "banana", "-5.0" } would be turned into a vector of three doubles.
	//If requireALLnumbers is set to true, then such input would cause an error to be thrown.

	//This function can be used to determine whether all the strings in a vector represent values that can be expressed as doubles.
	//Simply call the function in a "try" block, with requireALLnumbers set to true, and if no Exception is caught, then yes they all represent values!

	std::vector<double> values;

	for (size_t i = 0; i < v.size(); ++i) {
		try {
			double val = std::stod(v[i]);
			//We will only get to the following line if the entry is interpretable as a double.
			values.push_back(val);
		}
		catch (std::exception e) {
			if (requireALLnumbers) {
				throw e;
			}
		}
	}
	return values;
}

std::vector<int> Utility::toInts(const std::vector<std::string> v, bool requireALLints)
{
	//Input: A vector of any size, of strings that each represent a value that can be expressed as an int.
	//Output: A vector of those ints, in the same order as the original strings.

	//If requireALLints is set to false, then the returned vector will consist of only the entries that were interpretable as ints.
	//So for example, this vector of four strings { "3", "4", "banana", "-5.0" } would be turned into a vector of three ints.
	//If requireALLints is set to true, then such input would cause an error to be thrown.

	//This function can be used to determine whether all the strings in a vector represent values that can be expressed as ints.
	//Simply call the function in a "try" block, with requireALLints set to true, and if no Exception is caught, then yes they all represent ints!

	std::vector<int> values;

	for (size_t i = 0; i < v.size(); ++i) {
		try {
			double val = std::stoi(v[i]);
			//We will only get to the following line if the entry is interpretable as an int.
			values.push_back(val);
		}
		catch (std::exception e) {
			if (requireALLints) {
				throw e;
			}
		}
	}
	return values;
}