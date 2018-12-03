#pragma comment(lib, "Wininet")
#include <iostream>
#include <windows.h>
#include <WinInet.h>
#include <fstream>
using namespace std;

ifstream input;

int main()
{
	//char * user, *parola, *adresaServer;
	//cin >> adresaServer >> user >> parola;
	int err;
	auto internetHandle = InternetOpen("andreea", INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, NULL);
	if (!internetHandle)
	{
		cout << "Not opened.";
		return 0;
	}
	auto internetConnectionHandle = InternetConnect(internetHandle, "127.0.0.1",
		INTERNET_DEFAULT_FTP_PORT, NULL, NULL, INTERNET_SERVICE_FTP, NULL,
		0);
	if (!internetConnectionHandle)
	{
		cout << "No internet connection.";
		return 0;
	}

	WIN32_FIND_DATA findFileData = WIN32_FIND_DATA();
	auto findFileHandler = FtpFindFirstFileA(internetConnectionHandle, NULL, &findFileData,
		INTERNET_FLAG_RELOAD & INTERNET_FLAG_NEED_FILE, 0);
	err = GetLastError();
	if (err != 0)
	{
		cout << "Probleme find " << err << "\n";
		return 0;
	}

	cout << "What we got: " << findFileData.cFileName << "\n";

	auto readHandler = FtpOpenFileA(internetConnectionHandle, findFileData.cFileName, GENERIC_READ, FTP_TRANSFER_TYPE_ASCII, 0);
	err = GetLastError();
	if (err != 0)
	{
		cout << "Probleme open " << err << "\n";
		return 0;
	}

	char buffer[1000];
	DWORD read = 0;

	if (InternetReadFile(readHandler, buffer, 1000, &read) == 0)
	{
		cout << "Probleme read " << GetLastError() << "\n";
		return 0;
	}
	else
	{
		buffer[read] = '\0';
		cout << "File content:" << buffer << "\n";
	}

	InternetFindNextFileA(findFileHandler, &findFileData);
	cout << GetLastError();

	InternetCloseHandle(internetHandle);
	return 0;
}
