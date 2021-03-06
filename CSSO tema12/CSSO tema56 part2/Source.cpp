#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <string>
#include <sstream>

using namespace std;

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

std::string ReadMemory()
{
	auto file = OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, "CSSO");

	char* pData = (char*)MapViewOfFile(file, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pData == nullptr)
	{
		std::cout << "Cannot get pointer to file mapping. ", GetLastErrorAsString();
		CloseHandle(file);
		return "!!!Error!!!";
	}

	return std::string(pData);
}

HANDLE CreateEvent(std::string name)
{
	auto handler = CreateEvent(NULL, false, true, name.c_str());
	if (!handler)
		std::cout << "Error creating event 2: " << name << GetLastError() << ". Message:" << GetLastErrorAsString() << '\n';

	return handler;
}

HANDLE WaitEvent(std::string name)
{
	auto handler = OpenEvent(EVENT_ALL_ACCESS, false, name.c_str());

	if (!handler)
		std::cout << "Error waiting 2: " << name << GetLastError() << ". Message:" << GetLastErrorAsString() << '\n';
	WaitForSingleObject(handler, INFINITE);

	return handler;
}

void main()
{
	/*if (!AttachConsole(ATTACH_PARENT_PROCESS))
		std::cout << "Error attaching to console: " << GetLastError() << ". Message:" << GetLastErrorAsString();*/

	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ALL_ACCESS, &hToken))
		cout << "Error getting PID: " << GetLastError() << ". Message:" << GetLastErrorAsString();

	//SetPrivilege(hToken, "SeDebugPrivilege", true);

	//Sleep(2000);

	HANDLE doneEvent = nullptr;
	int count = 0;
	while (true) {

		Sleep(10);

		if(!CloseHandle(WaitEvent("write")))
			cout << "Error closing event write 2: " << GetLastError() << ". Message:" << GetLastErrorAsString() << '\n';

		if (doneEvent && !CloseHandle(doneEvent))
			std::cout << "Error closing event done 2: " << GetLastError() << ". Message:" << GetLastErrorAsString() << '\n';

		auto input = ReadMemory();

		istringstream ss(input);

		string first, second;

		getline(ss, first, ' ');
		getline(ss, second, ' ');

		int a = stoi(first);
		int b = stoi(second);

		if (b == a * 2)
			cout << ++count << " :" << a << "*2==" << b << "\n";

		doneEvent = CreateEvent("done");
		if (!SetEvent(doneEvent))
			std::cout << "Error creating event done 2: " << GetLastError() << ". Message:" << GetLastErrorAsString() << '\n';
	}
}