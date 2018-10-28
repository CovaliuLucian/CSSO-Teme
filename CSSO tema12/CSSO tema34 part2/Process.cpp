#include "Process.h"
#include <iostream>

Process::Process(std::string data)
{
	map(data);
	check = false;
}

void Process::map(std::string data)
{
	std::string delimiter = "--";
	int count = 0;
	size_t pos;
	std::string token;
	while ((pos = data.find(delimiter)) != std::string::npos) {
		token = data.substr(0, pos);
		switch (count)
		{
		case 0:
			this->PPID = std::stoi(token);
			break;
		case 1:
			this->PID = std::stoi(token);
			break;
		}
		count++;
		data.erase(0, pos + delimiter.length());
	}

	this->exe = data;
}
