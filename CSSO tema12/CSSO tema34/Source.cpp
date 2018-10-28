#include <Windows.h>
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

void main()
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

	std::string testdata = "abc";
	//memcpy(pData, testdata.c_str(), testdata.size());
	strcpy_s(pData, testdata.length() + 1, testdata.c_str());

	std::cout << "Done";

	std::cin.ignore();
}
