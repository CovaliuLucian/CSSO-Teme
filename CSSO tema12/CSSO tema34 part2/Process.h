#pragma once
#include <string>

class Process
{
public:
	int PID;
	int PPID;
	std::string exe;

	Process(std::string data);

	Process(int pid, int ppid, const std::string& exe)
		: PID(pid),
		  PPID(ppid),
		  exe(exe)
	{
	}

	void map(std::string data);
};
