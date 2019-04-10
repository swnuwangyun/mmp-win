#ifndef PMXV_LOGGER_H
#define PMXV_LOGGER_H

#include <fstream>
#include <string>

class pmxvLogger
{
public:
	static pmxvLogger* get();

	pmxvLogger();
	~pmxvLogger();
	
	void setLogPath(const std::string & path);
	void e(const std::string &error);
	void d(const std::string &debug);
	void m(const std::string &message);
	
protected:
	std::ofstream* logfile;
};

#define err pmxvLogger::get()->e
#define dbg pmxvLogger::get()->d
#define log pmxvLogger::get()->m

#endif // PMXV_LOGGER_H

