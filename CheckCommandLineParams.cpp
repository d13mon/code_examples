
#include <pcre.h>

#include <iostream>
#include <tuple>


static const int kMaxPcreMatchesCount = 30;

enum class CheckParamsError {
	kSuccess = 0,
	kFormatError = -1	
};

std::pair<bool, std::string> check_string_format(const std::string& str, const std::string& pattern) {
	const char* error = nullptr;
	int error_offset = 0;
	int string_count = 0;
	int ovector[kMaxPcreMatchesCount];	

	pcre* re;
	if ((re = pcre_compile(pattern.c_str(), 0, &error, &error_offset, NULL)) == NULL) {
		return { false, "Pattern compilation error" };
	}

	bool result = false;
	const auto length = static_cast<int>(strlen(str.c_str()));
	if ((string_count = pcre_exec(re, NULL, str.c_str(), length, 0, 0, ovector, kMaxPcreMatchesCount)) < 0) {
		return { false, "Pattern not found" };
	}

	// std::cout << "String count = " << string_count << std::endl;

	pcre_free(re);

	return { true, {} };
}


bool check_job_id(const std::string& job_id) {	
	//NOTE: Format is YYJJJNNNNNNN 	
	const std::string pattern = "^([0-9]{2})([0-2][0-9]{2}|3[0-5][0-9]|36[0-6])([0-9]{7})$";

	bool success;
	std::string message;
	std::tie(success, message) = check_string_format(job_id, pattern);

	return success;
}

static std::string get_date_pattern() {
	//NOTE: Format is DDMMYYYY

	std::string pattern;

	//DD
	pattern += "(0[1-9]|[1-2][0-9]|3[0-1])";

	//MM
	pattern += "(0[1-9]|1[0-2])";

	//YYYY
	pattern += "(19[7-9][0-9]|20[0-9]{2})";

	return pattern;
}

static std::string get_time_pattern() {
	//NOTE: Format is HHMMSS

	std::string pattern;

	//HH
	pattern += "([0-1][0-9]|2[0-3])";

	//MM
	pattern += "([0-5][0-9])";

	//SS
	pattern += "([0-5][0-9])";

	return pattern;
}

bool check_date_time(const std::string& date_time) {
	//NOTE: Format is DDMMYYYYHHMMSS (Correct range is 01011970000000 - 31122099235959)

	const std::string pattern = "^" + get_date_pattern() + get_time_pattern() + "$";

	bool success;
	std::string message;
	std::tie(success, message) = check_string_format(date_time, pattern);	

	return success;
}

bool check_date(const std::string& date) {
	//NOTE: Format is DDMMYYYY (Correct range is 01011970 - 31122099)

	const std::string pattern = "^" + get_date_pattern() + "$";

	bool success;
	std::string message;
	std::tie(success, message) = check_string_format(date, pattern);

	return success;
}


#if !ENABLE_TESTS
int main()
{
    using namespace std;

    cout << "FORMAT: " << (check_date_time("12072021150059") ? "OK" : "WRONG") << endl;
}
#endif
