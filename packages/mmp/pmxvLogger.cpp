#include "pmxvLogger.h"
#include <assert.h>
#include <iostream>

const std::string CONST_DEFAULT_LOGNAME = "yymmplog.txt";

pmxvLogger::pmxvLogger()
{
}

pmxvLogger* pmxvLogger::get()
{
	static pmxvLogger *instance = 0;
	
	if (!instance)
		instance = new pmxvLogger();
		
	return instance;
}
	
pmxvLogger::~pmxvLogger()
{
	logfile->close();
	delete logfile;
}

void pmxvLogger::setLogPath(const std::string & path)
{
	std::string logfilename = path.empty() ? CONST_DEFAULT_LOGNAME : (path + "\\" + CONST_DEFAULT_LOGNAME);
	logfile = new std::ofstream();
	logfile->open(logfilename, std::ios::app | std::ios::out);
	assert(logfile->is_open());
}

void pmxvLogger::e(const std::string &error)
{
	static std::string msg;
	msg = "error: " + error + '\n';
	*logfile  << msg;
	logfile->flush();
	std::cerr << msg;
}

void pmxvLogger::d(const std::string &debug)
{
#ifdef DEBUG
	static std::string msg;
	msg = "debug: " + error + '\n';
	*logfile  << msg;
	logfile->flush();
	std::cout << msg;
#endif
}

void pmxvLogger::m(const std::string &message)
{
	static std::string msg;
	msg = message + '\n';
	*logfile  << msg;
	logfile->flush();
	std::cout << msg;
}
