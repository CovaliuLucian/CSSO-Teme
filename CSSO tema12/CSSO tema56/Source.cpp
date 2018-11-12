#include <Windows.h>
#include <string>
#include <iostream>

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

void main()
{
	WriteMemory("fuck off");

	PROCESS_INFORMATION infos;
	STARTUPINFO start;
	ZeroMemory(&start, sizeof start);
	start.cb = sizeof(start);
	ZeroMemory(&infos, sizeof infos);

	if(!CreateProcess(NULL, "\"C:\\Users\\lucia\\Source\\Repos\\CSSO-Teme\\CSSO tema12\\Debug\\CSSO tema56 part2.exe\"", NULL, NULL, false, 0, NULL, NULL, &start, &infos))
	{
		std::cout << "Error creating process: " << GetLastError() << "\n";
	}

	CloseHandle(infos.hProcess);
	CloseHandle(infos.hThread);

	std::cout << "Done, waiting...\n";

	std::cin.ignore();
}