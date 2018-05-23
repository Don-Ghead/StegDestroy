/*
ADD DESCRIPTION
*/

#include "stdafx.h"
#include "Srl_steg_logger.hpp"

#include <iostream>
#include <Windows.h>
#include <stdio.h>


using namespace std;

//Not used here but keeping it as could be useful in the future for linux
const vector<string> sysinfo_tokens
{
	"model name",
	"cpu cores"
};

//Don't need to check the log level as it is enumerated and must be on of the specified values
steg_logger::steg_logger(Log_level log_lvl, string altlog_filename)
	: m_log_level(log_lvl), m_permalog_filename(perma_log_filepath), m_using_altlog(false), m_details_outstanding(false)
{
	if (!altlog_filename.empty())
	{
		m_alternatelog_filename = altlog_filename;
		m_using_altlog = true;
	}

	//Automatically get the sysinfo string as the first thing we do in the logger
	//Will be the first part of all the sysinfo entries
	string sysinfo = get_sysinfo_string();
	m_logfile_details.push_back(sysinfo);
	m_details_outstanding = true;
}

steg_logger::~steg_logger()
{
	if (m_details_outstanding)
	{
		write_logdetails_to_path();
	}
}

void steg_logger::add_logfile_detail(string log_detail)
{
	m_logfile_details.push_back(log_detail);
	m_details_outstanding = true;
}

//Write the m_logfile_details to the provided path, if path is not provided 
//Writes instead to the permalog & altlog if there is one
bool steg_logger::write_logdetails_to_path(string logpath)
{
	bool success = false;
	if (!logpath.empty()) //Write only to this path 
	{
		//Open output stream in output/append mode
		ofstream logfile(logpath, ios::out | ios::app);

		if (logfile.is_open())
		{
			cout << "Writing details to " << logpath << endl;
			//Divider includes newlines on either side to ensure entries are divided 
			logfile << logfile_entry_divider;

			for (int i = 0; i < m_logfile_details.size(); i++)
			{
				logfile << m_logfile_details[i] << ", ";
			}
			logfile << endl;
			logfile.close();
			success = true;
		}
		else
		{
			cout << "Unable to open path to: " + logpath;
		}

	}
	else //Write to perma/alt 
	{
		if (m_using_altlog)
		{
			ofstream alt_logfile(m_alternatelog_filename, ios::out | ios::app);

			if (alt_logfile.is_open())
			{
				cout << "Writing details to alt & perma logs" << endl;
				//Divider includes newlines on either side to ensure entries are divided 
				alt_logfile << logfile_entry_divider;

				for (int i = 0; i < m_logfile_details.size(); i++)
				{
					alt_logfile << m_logfile_details[i] << ", ";
				}
				alt_logfile << endl;
				success = true;
			}
			else
			{
				cout << "Unable to open path to: " + m_alternatelog_filename;
			}
		}
		ofstream logfile(m_permalog_filename, ios::out | ios::app);

		if (logfile.is_open())
		{
			cout << "Writing details to permalog only" << endl;
			//Divider includes newlines on either side to ensure entries are divided 
			logfile << logfile_entry_divider;

			for (int i = 0; i < m_logfile_details.size(); i++)
			{
				logfile << m_logfile_details[i] << ", ";
			}
			logfile << endl;
			success = true;
		}
		else
		{
			cout << "Unable to open path to: " + m_permalog_filename;
		}
	}

	if (success)
	{
		m_details_outstanding = false;
	}
	else
	{
		m_details_outstanding = true;
	}
	return success;
}

//This is only for Unix systems though works agnostically if setup correctly
//Utility function for get_sysinfo_string for unix platforms
string steg_logger::extract_info_from_sysstring(string extraction_string, vector<string> tokens_to_match)
{
	if (tokens_to_match.empty() || extraction_string.empty())
	{
		//return empty string 
		return string("");
	}
	else
	{
		for (int i = 0; i < tokens_to_match.size(); i++)
		{
			//Search for token, if found return information after colon
			size_t found = extraction_string.find_last_of(tokens_to_match[i]);
			//Npos indicates we didn't find the character
			if (string::npos != found)
			{
				found = extraction_string.find_last_of(":");
				if (string::npos != found)
				{
					return extraction_string.substr(found);
				}
			}
		}
	}
	return "";
}

/*
This pulls information from the platform specific method
Unix = /proc/cpuinfo
Windows = getsysinfo
*/
string steg_logger::get_sysinfo_string(void)
{
	string return_string("");

	SYSTEM_INFO siSysInfo;

	// Copy the hardware information to the SYSTEM_INFO structure. 
	GetSystemInfo(&siSysInfo);

	return_string += "Windows Hardware Information:\\r\\n";
	return_string += "Processor type: " + siSysInfo.dwProcessorType;
	return_string += "Number of Processors: " + siSysInfo.dwNumberOfProcessors;

	return return_string;
}


//May add to logger later on if i feel it's necessary/useful otherwise i'll just get rid of it.
/*
std::cout << "Underlying Matrix structure for " << image_names[i] << endl << endl;
std::cout << "Matrix Colour Depth: ";

int bit_depth, bit_depth_max;
switch (cv_images[i].depth())
{

case CV_8U:
bit_depth = 8;
bit_depth_max = CHAR_MAX;
break;

case CV_8S:
bit_depth = 7;
bit_depth_max = UCHAR_MAX;
break;

case CV_16U:
bit_depth = 16;
bit_depth_max = SHRT_MAX;
break;

case CV_16S:
bit_depth = 15;
bit_depth_max = USHRT_MAX;
break;

default:
cout << "Encountered 32bit float or signed integer" << endl;
}
/*cout << bit_depth << endl << "Max pixel value: " << bit_depth_max << endl;
cout << "Number of channels for image type: " << [i].channels() << endl;
Size cvmat_size = images_original[i].size();
cout << "Height (Number of rows): " << cvmat_size.height << endl << "Width (Number of columns): " << cvmat_size.width << endl;
cout << "Raw image size (in bytes) = (height * width * channels) * sizeof(colour depth)" << endl;
cout << "Image size (in kb): " << ( cvmat_size.height * cvmat_size.width * images_original[i].channels() )/1000 << endl;
cout << text_divider << endl;
*/
