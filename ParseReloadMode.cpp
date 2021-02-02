// ParseReloadMode.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <regex>
#include "regex_exception_codes.hpp"

#include <vector>


const char* default_space_chars = " \t\n\r\f\v";

//trim from the end of the string
inline std::string& rtrim(std::string& s, const char* space_chars = default_space_chars) {
	s.erase(s.find_last_not_of(space_chars) + 1);
	return s;
}

//trim from the beginning of the string
inline std::string& ltrim(std::string& s, const char* space_chars = default_space_chars) {
	s.erase(0, s.find_first_not_of(space_chars));
	return s;
}

//trim from both ends of the string
inline std::string& trim(std::string& s, const char* space_chars = default_space_chars) {
	return ltrim(rtrim(s, space_chars), space_chars);
}


struct CommandLineParameters {
    std::string reformPostingFilename;
    std::vector<std::string> txnIdsList;
};

#include <sstream>

template<typename InputIterator>
std::string join(InputIterator begin, InputIterator end,
	             const std::string& separator = ", ",
	             const std::string& concluder = "")
{
	if (begin == end) {
		return concluder;
	}

	std::stringstream ss;
	ss << *begin;
	++begin;

	while (begin != end) {
		ss << separator;
		ss << *begin;
		++begin;
	}

	ss << concluder;

	return ss.str();
}

std::string join(const std::vector<std::string>& vec, 
	             const std::string& separator = ", ",
	             const std::string& concluder = "") {
	return join(vec.cbegin(), vec.cend(), separator, concluder);
}



bool parse_reload_mode_arguments(std::string arg, CommandLineParameters* params) {

	using namespace std;

	//TODO:
	trim(arg);

	try {
		//const regex regex_filename("^-file\\s+([_\\w\\d\\.]+)$");
		const regex regex_filename(R"(^-file\s+([_\w\d\.]+)$)");

		const regex regex_txnid_list("^-txn\\s+\\{((\\d+;)*(\\d+))\\}$");		
			
		smatch match_info;

		if (regex_match(arg, match_info, regex_filename)) {
			params->reformPostingFilename = match_info[1].str();
			return true;
		}
		else if (regex_match(arg, match_info, regex_txnid_list)) {
			const regex regex_txnid("\\d+");
			const auto txnid_list_str = match_info[1].str();

			match_info = {};
			auto pos = txnid_list_str.cbegin();
			auto end = txnid_list_str.cend();

			while(regex_search(pos, end, match_info, regex_txnid)) { 				
				params->txnIdsList.push_back(match_info.str(0));
				pos = match_info.suffix().first;
			}			
				
			return true;
		}

		return false;
	}
	catch (const std::regex_error& e) {
		//TODO:
		std::cout << "regex_error caught: " << e.what()  << " | " << get_regex_error_text(e.code()) << '\n';
	}	

    return false;
}

#if !MY_TEST 


int main()
{
	using namespace std;
	
	CommandLineParameters params;
	cout << parse_reload_mode_arguments("  -file cpsYYDDD_0.cp999  ", &params) << "\n";	
    std::cout << "filename=" << params.reformPostingFilename << '\n';

	cout << parse_reload_mode_arguments(" -txn {123;456;789}", &params) << "\n";
	//std::cout << "filename=" << params.reformPostingFilename;
}

#endif