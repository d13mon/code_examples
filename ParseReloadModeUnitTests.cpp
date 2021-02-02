#include "pch.h"

#define MY_TEST 1

#include "ParseReloadMode.cpp"

#include <vector>
#include <tuple>

TEST(Test_ConvertTrim, TestLTrim) {

	std::vector<std::string> testDataList = { 
		" test ", "   ", "\t\r ", "abcdef ", "__abcd", {}, "\nabc abc", "\v\f\t_x_\t\t" 
	};

	const std::vector<std::string> expectedList = { 
		"test ", {}, {}, "abcdef ", "__abcd", {}, "abc abc", "_x_\t\t" 
	};

	const auto testsCount = testDataList.size();

	for (size_t i{0}; i < testsCount; ++i) {
		auto s = ltrim(testDataList[i]);
		EXPECT_EQ(testDataList[i], expectedList[i]);
		EXPECT_EQ(s, expectedList[i]);
	}
}

TEST(Test_ConvertTrim, TestRTrim) {

	std::vector<std::string> testDataList = { 
		" test ", "   ", "\t\r ", "abcdef ", "__abcd\n\f", {}, "\nabc abc", "\v\f\t_x_\t\t" 
	};

	const std::vector<std::string> expectedList = { 
		" test", {}, {}, "abcdef", "__abcd", {}, "\nabc abc", "\v\f\t_x_" 
	};

	const auto testsCount = testDataList.size();

	for (size_t i{0}; i < testsCount; ++i) {
		auto s = rtrim(testDataList[i]);
		EXPECT_EQ(testDataList[i], expectedList[i]);
		EXPECT_EQ(s, expectedList[i]);
	}
}

TEST(Test_ConvertTrim, TestTrim) {
 
	std::vector<std::string> testDataList = {
		" test ", "   ", "\t\r ", "abcdef ", "__abcd", {}, "\nabc abc", "\v\f\t_x_\t\t"
	};

	const std::vector<std::string> expectedList = { 
		"test", {}, {}, "abcdef", "__abcd", {}, "abc abc", "_x_" 
	};

	const auto testsCount = testDataList.size();

	for (size_t i{0}; i < testsCount; ++i) {
	    auto s = trim(testDataList[i]);
        EXPECT_EQ(testDataList[i], expectedList[i]);
		EXPECT_EQ(s, expectedList[i]);
	}   
}

TEST(Test_ConvertJoin, TestJoinDefault) {

	//vector
	using InputData = std::vector<std::string>;

	std::vector<InputData> testDataList = {
		{"a", "b", "c"},
		{"some", {}, "string"},
		{"435", "", "223", "u8", "111"}, 
		{}
	};

	const std::vector<std::string> expectedList = {
		"a, b, c",
		"some, , string",
		"435, , 223, u8, 111",
		{}
	};

	const auto testsCount = testDataList.size();

	for (size_t i{ 0 }; i < testsCount; ++i) {
		auto joined = join(testDataList[i]);
		EXPECT_EQ(joined, expectedList[i]);
	}
}


TEST(Test_ConvertJoin, TestJoinCommon) {

	//vector, separator, concluder
	using InputData = std::tuple<std::vector<std::string>, std::string, std::string>; 

	std::vector<InputData> testDataList = {
		{ {"a", "b", "c"}, ";", "_end"},
		{ {"some", {}, "string"}, {}, {} },
		{ {"435", "", "223", "u8", "111"}, ", ", "s" },
		{ {}, ";", "Y"}
	};

	const std::vector<std::string> expectedList = {
		"a;b;c_end",
		"somestring",
		"435, , 223, u8, 111s",
		"Y"
	};

	const auto testsCount = testDataList.size();

	for (size_t i{ 0 }; i < testsCount; ++i) {
		std::vector<std::string> list;
		std::string sep, con;
		std::tie(list, sep, con) = testDataList[i];
		auto joined = join(list.cbegin(), list.cend(), sep, con);
		EXPECT_EQ(joined, expectedList[i]);
	}

	for (size_t i{ 0 }; i < testsCount; ++i) {		
		auto joined = join(std::get<0>(testDataList[i]), std::get<1>(testDataList[i]), std::get<2>(testDataList[i]));		
		EXPECT_EQ(joined, expectedList[i]);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////



TEST(TestReloadMode, TestFormat) {

	const std::vector<std::string> testDataList = {
		"",
		" -file 12345",
		"12345",
		"--file vghfg.h",
		" -file",
		"-file 12345 4",
		"-file 12345, 45",
		"  -file  cps_ctfRRR_X.cp000 ",
		"-file cps_ctf&&RRR_X.cp000",
		"file 22345",

		"-File 54657",
		"-file 3445 -txn {46546757}",
		"-txn ",
		"-txn 436546;768979",
		"--txn {54657;769889}",
		"-txn {46547;67978978}",
		"-txn {46547; 67978978}",
		" -txn  {46547;67978978} ",
		"{46547;67978978}",
		"-Txn {46547;67978978}",

		"-txn {}",
		"-txn {;}",
		"-tx n {65767;89;88}", 
		"   -txn {54675767;678768;67878;7876876;65765;657567;5467567;657867;678768}  ",
		"-txn { 6576;768678 }",
		"-file fgf;fghg",
		"-txn {6576;768678;}",
		"-txn [6576;768678]",
	};

	const std::vector<bool> expectedList = { 
		false, true, false, false, false, false, false, true, false, false, 
		false, false, false, false, false, true, false, true, false, false, 
		false, false, false, true, false , false, false, false
	};

	const auto testsCount = testDataList.size();

	for (size_t i{ 0 }; i < testsCount; ++i) {
		CommandLineParameters params;
		auto result = parse_reload_mode_arguments(testDataList[i], &params);		
		EXPECT_EQ(result, expectedList[i]);		
	}
}

TEST(TestReloadMode, TestReloadWithFilename) {

	const std::vector<std::string> testDataList = {
		"-file 12345",	
		"-file cpsYYDDD_0.cp999"
	};

	const std::vector<std::string> expectedList = {
		"12345", "cpsYYDDD_0.cp999"
	};

	const auto testsCount = testDataList.size();

	for (size_t i{ 0 }; i < testsCount; ++i) {
		CommandLineParameters params;
		auto result = parse_reload_mode_arguments(testDataList[i], &params);
		EXPECT_TRUE(result);
		EXPECT_EQ(params.reformPostingFilename, expectedList[i]);
	}
}

TEST(TestReloadMode, TestReloadWithTxnIdList) {

	const std::vector<std::string> testDataList = {
		"-txn {12346}",
		" -txn {123;456;789}",
		" -txn  {54675767;678768;67878;7876876;65765;657567;5467567;657867;678768}"
	};

	const std::vector<std::vector<std::string>> expectedList = {
		{"12346"},
		{"123", "456", "789"},
		{"54675767", "678768", "67878", "7876876", "65765", "657567", "5467567", "657867", "678768"}
	};

	const auto testsCount = testDataList.size();

	for (size_t i{ 0 }; i < testsCount; ++i) {
		CommandLineParameters params;
		auto result = parse_reload_mode_arguments(testDataList[i], &params);
		EXPECT_TRUE(result);
		EXPECT_EQ(params.txnIdsList, expectedList[i]);
	}
}