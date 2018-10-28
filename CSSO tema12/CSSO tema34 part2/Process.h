#pragma once
#include <string>

class Process
{
public:
	int PID;
	int PPID;
	std::string exe;
	bool check;

	Process(std::string data);

	Process(int pid, int ppid, const std::string& exe)
		: PID(pid),
		  PPID(ppid),
		  exe(exe),
		  check(false)
	{
	}

	void map(std::string data);
};
