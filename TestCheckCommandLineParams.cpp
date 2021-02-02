#include "pch.h"

#define ENABLE_TESTS 1

#include "CheckCommandLineParams.cpp"

#include <vector>
#include <iostream>

TEST(TestCheckCommandLineParams, TestJobIdFormat) {
	std::vector<std::string> testDataList = {
		"201011234567", "1234567", "123456789012", "203650000000", "991111234567",
		"001111234567", "20202123456", "401111234567", "203662345678", "204671234567",
		"2036712344567", "206781234567", "990011234567", "202029999999", ""
	};

	const std::vector<bool> expectedList = {
		true, false, true, true, true, 
		true, false, true, true, false,
		false, false, true, true, false
	};

	const auto testsCount = testDataList.size();

	for (size_t i{ 0 }; i < testsCount; ++i) {
		auto test_case = testDataList[i];
		std::cout << "Test case = " << test_case << std::endl;
		auto result = check_job_id(test_case);		
		EXPECT_EQ(result, expectedList[i]);
	}
}

TEST(TestCheckCommandLineParams, TestDateTime) {
	std::vector<std::string> testDataList = {
		"", "12445", "2101200", "21012020",  "20210101",
		"120720200", "29031999", "33072018", "00041977", "01011970",
		"01001998", "29021988", "11112011", "11152022", "31122099",
		"01012100", "01011969", "17011899", "30081987", "11112051",
		"01012000", "09122000"
	};

	const std::vector<bool> expectedList = {
		false, false, false, true, false,
		false, true, false, false, true,
		false, true, true, false, true, 
		false, false, false, true, true,
		true, true
	};

	const auto testsCount = testDataList.size();

	for (size_t i{ 0 }; i < testsCount; ++i) {
		auto test_case = testDataList[i];
		std::cout << "Test case = " << test_case << std::endl;
		auto result = check_date(test_case);
		EXPECT_EQ(result, expectedList[i]);
	}
}

TEST(TestCheckCommandLineParams, TestDateTimeFormat) {
	std::vector<std::string> testDataList = {
		"", "12", "210120001215", "210120201400000",  "20210101130000", 
		"12072020134759", "29031999110020", "33072018110000", "00041977133333", "01011970000000",
		"01001998235959", "29021988235959", "11112011000000", "11152022010101", "31122099235959",
		"01012100111111", "01011969000000", "17011899180404", "30081987240000", "11112011000100",
		"01012000304343", "01012000115363", "01012001119333"
	};

	const std::vector<bool> expectedList = {
		false, false, false, false, false,
		true, true, false, false, true,
		false, true, true, false, true,
		false, false, false, false, true,
		false, false, false
	};

	const auto testsCount = testDataList.size();

	for (size_t i{ 0 }; i < testsCount; ++i) {
		auto test_case = testDataList[i];
		std::cout << "Test case = " << test_case << std::endl;
		auto result = check_date_time(test_case);
		EXPECT_EQ(result, expectedList[i]);
	}
}