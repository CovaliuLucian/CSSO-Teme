#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>

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

std::string readProcesses()
{
	HANDLE hProcess = nullptr;
	PROCESSENTRY32 pe32;

	std::string toReturn = std::string();

	//cer un snapshot la procese
	auto hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot failed.err = %d \n", GetLastError());
		return toReturn;
	}
	//initializez dwSize cu dimensiunea structurii.
	pe32.dwSize = sizeof(PROCESSENTRY32);
	//obtin informatii despre primul proces
	if (!Process32First(hProcessSnap, &pe32))
	{
		printf("Process32First failed. err = %d \n", GetLastError());
		CloseHandle(hProcessSnap); //inchidem snapshot-ul
		return toReturn;
	}
	do
	{
		char* process = new char[200];
		sprintf_s(process, 200, "%d--%d--%s\n", pe32.th32ParentProcessID, pe32.th32ProcessID, pe32.szExeFile);
		toReturn.append(process);
	}
	while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcess);

	return toReturn;
}

void main()
{
	std::string processData = readProcesses();

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

	strcpy_s(pData, processData.length() + 1, processData.c_str());

	std::cout << "Done";

	std::cin.ignore();
}
