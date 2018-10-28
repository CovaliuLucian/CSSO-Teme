#include <iostream>
#include <Windows.h>
#include <string>
#include "Process.h"
#include <list>

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

BOOL SetPrivilege(
	HANDLE hToken, // access token handle
	LPCTSTR lpszPrivilege, // name of privilege to enable/disable
	BOOL bEnablePrivilege // to enable or disable privilege
)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(
		NULL, // lookup privilege on local system
		lpszPrivilege, // privilege to lookup 
		&luid)) // receives LUID of privilege
	{
		std::cout << "LookupPrivilegeValue error: ", GetLastErrorAsString();
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		std::cout << "AdjustTokenPrivileges error: ", GetLastErrorAsString();
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

	{
		printf("The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}

std::list<Process*> MapData(std::string data)
{
	std::list<Process*> list = std::list<Process*>();

	std::string delimiter = "\n";
	int count = 0;
	size_t pos;
	std::string token;
	while ((pos = data.find(delimiter)) != std::string::npos)
	{
		token = data.substr(0, pos);
		auto process = new Process(token);
		list.push_back(process);
		count++;
		data.erase(0, pos + delimiter.length());
	}

	return list;
}

void main()
{
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(),
	                      TOKEN_ALL_ACCESS, &hToken))
		return;

	SetPrivilege(hToken, "SeDebugPrivilege", true);

	auto file = OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, "CSSO");

	char* pData = (char*)MapViewOfFile(file, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pData == nullptr)
	{
		std::cout << "Cannot get pointer to file mapping. ", GetLastErrorAsString();
		CloseHandle(file);
		return;
	}

	auto list = MapData(std::string(pData));

	for (auto process : list)
	{
		std::cout << process->PID << " + " << process->exe << "\n";
	}
}
