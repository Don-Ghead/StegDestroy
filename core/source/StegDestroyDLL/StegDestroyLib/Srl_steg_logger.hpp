#pragma once

//TODO - Add an openMP version to the sysinfo log

#ifndef _SRL_STEG_LOGGER_HPP
#define _SRL_STEG_LOGGER_HPP

#include <fstream>
#include <chrono>
#include <string>
#include <vector>

using namespace std;


const string perma_log_filepath("..\\resources\\logs\\perma_log.txt");

//Divides every new entry to the logfile (between runs)
const string logfile_entry_divider("\\r\\n#NEW_ENTRY\\r\\n");


enum Log_level {
	NONE = 0,
	MINIMUM = 1,
	DEFAULT = 2,
	HIGH = 3
};

class steg_logger
{
private:

	/////////////////////
	//PRIVATE MEMBERS  //
	/////////////////////

	//Stores the permanent log filename. Always uses Log_level HIGH
	string m_permalog_filename;

	//May be empty 
	string m_alternatelog_filename;

	//Quick access for determining whether to also write to the alternate log
	bool m_using_altlog;

	//Contains the details to be written to the logfile, vectored to reduce 
	//the need for constant file IO. we'll open + write all of it in one go. 
	//All entries will be comma seperated in a single entry, that way in the 
	//Future, maybe just use Excel to handle them 
	vector<string> m_logfile_details;

	//Determines the amount of information to be logged to the system
	//For this project will only check if NONE otherwise will just log all
	Log_level m_log_level;

	/////////////////////
	//PRIVATE FUNCTIONS//
	/////////////////////

	//Get the sysinfo string from either unix or windows, automatically determined
	string get_sysinfo_string(void);

	//extract information from a unix sysinfo string given a set of tokens check /proc/cpuinfo 
	string extract_info_from_sysstring(string extraction_string, vector<string> tokens_to_match);

	//this will be true when there are strings in the logfile details member to be written still 
	//And allows the destructor to manage writing those to disk before they are lost
	bool m_details_outstanding;

public:

	/************************************************

	CTOR + DTOR

	*************************************************/

	//If altlog_filename provided writes the details to a seperate logfile as well
	//otherwise uses default permalog path only
	steg_logger(Log_level log_lvl, string altlog_filename = "");

	~steg_logger();

	/************************************************

	UTILITY

	*************************************************/

	//Adds a single string detail to m_logfile_details, used for most of the fractal 
	//generation details that we don't need to store in this class 
	void add_logfile_detail(string log_detail);


	/************************************************

	CORE

	*************************************************/

	//Write the m_logfile_details to the provided path, if path is not provided 
	//Writes instead to the permalog & altlog if there is one
	bool write_logdetails_to_path(string logpath = "");
};

#endif

#pragma once
