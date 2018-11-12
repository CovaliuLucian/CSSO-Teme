#include <Windows.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>

std::string* GetLastErrorAsString()
{
	DWORD errorMessageID = GetLastError();
	if (errorMessageID == 0)
		return new std::string();

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, nullptr);

	return new std::string(messageBuffer, size);
}

void WriteMemory(std::string text)
{
	HANDLE hData = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, 1024 * 1024, "CSSO");
	if (hData == nullptr)
	{
		std::cout << "Cannot create file mapping. ", GetLastErrorAsString();
		return;
	}

	char* pData = (char*)MapViewOfFile(hData, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pData == nullptr)
	{
		std::cout << "Cannot get pointer to file mapping. ", GetLastErrorAsString();
		CloseHandle(hData);
		return;
	}

	strcpy_s(pData, text.length() + 1, text.c_str());
}

std::string GetRandom()
{
	//srand(time(nullptr));
	const auto a = rand() % 100;
	const auto sum = a * 2;

	char buffer[20];

	snprintf(buffer, 20, "%i %i", a, sum);
	return std::string(buffer);
}

HANDLE CreateEvent(std::string name)
{
	auto handler = CreateEvent(NULL, false, true, name.c_str());
	if (!handler)
		std::cout << "Error creating event: " << name << GetLastError() << ". Message:" << GetLastErrorAsString() << '\n';

	return handler;
}

HANDLE WaitEvent(std::string name)
{
	auto handler = OpenEvent(EVENT_ALL_ACCESS, false, name.c_str());

	if (!handler)
		std::cout << "Error waiting 1: " << name << GetLastError() << ". Message:" << GetLastErrorAsString() << '\n';
	WaitForSingleObject(handler, INFINITE);

	return handler;
}

void main()
{
	PROCESS_INFORMATION infos;
	STARTUPINFO start;
	ZeroMemory(&start, sizeof start);
	start.cb = sizeof(start);
	ZeroMemory(&infos, sizeof infos);


	if (!CreateProcess(NULL, "\"C:\\Users\\lucia\\Source\\Repos\\CSSO-Teme\\CSSO tema12\\Debug\\CSSO tema56 part2.exe\"",
	                   NULL, NULL, false, 0, NULL, NULL, &start, &infos))
	{
		std::cout << "Error getting PID: " << GetLastError() << ". Message:" << GetLastErrorAsString() << '\n';
	}

	CloseHandle(infos.hProcess);
	CloseHandle(infos.hThread);
	bool first = true;
	HANDLE writeEvent = nullptr;
	CreateEvent("done");


	for (int i = 0; i < 200; ++i)
	{
		if (!first)
		{
			if (!CloseHandle(WaitEvent("done")))
				std::cout << "Error closing event done 1: " << GetLastError() << ". Message:" << GetLastErrorAsString() << '\n';
			if (!CloseHandle(writeEvent))
				std::cout << "Error closing event write 1: " << GetLastError() << ". Message:" << GetLastErrorAsString() << '\n';
		}

		first = false;
		WriteMemory(GetRandom());

		writeEvent = CreateEvent("write");
		if (!SetEvent(writeEvent))
			std::cout << "Error setting event write 1: " << GetLastError() << ". Message:" << GetLastErrorAsString() << '\n';

		//std::cout << i + 1 << ".Done, waiting...\n";
		//Sleep(10);
	}

	Sleep(1000);

	std::cin.ignore();
}
