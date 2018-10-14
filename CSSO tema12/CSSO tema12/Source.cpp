#include<Windows.h>
#include <iostream>

void CreateKey(HKEY hKey, std::string name)
{
	DWORD size = 10; // dummy value
	RegSetValueEx(
		hKey,
		name.c_str(),
		0,
		REG_DWORD,
		reinterpret_cast<const BYTE*>(&size),
		sizeof(DWORD));
}

HKEY CreateDirectory(std::string path)
{
	HKEY hKey;
	DWORD disposition = 0;
	std::string dirPath = "";
	path == "*" ? dirPath = "SOFTWARE\\CSSO\\tema1" : dirPath = "SOFTWARE\\CSSO\\tema1\\" + path;

	if (RegCreateKeyEx(
		HKEY_CURRENT_USER,
		dirPath.c_str(),
		0,
		nullptr,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		nullptr,
		&hKey,
		&disposition) != ERROR_SUCCESS)
	{
		printf("Nu s-a putut creea cheia. Cod eroare: %d\n", GetLastError());
	}

	return hKey;
}

std::string CreatePath(std::string path, std::string file)
{
	return file == "" ? path : file + "\\" + path;
}

void ListFiles(std::string path, std::string dirPath)
{
	HKEY hkey = CreateDirectory(path.substr(0, path.size() - 2));

	auto file = WIN32_FIND_DATA();
	std::string searchPath = dirPath == "" ? "*" : dirPath + "//*";
	auto handle = FindFirstFile(searchPath.c_str(), &file);
	//auto handle = FindFirstFile(path.c_str(), &file);

	while (FindNextFile(handle, &file))
	{
		std::cout << file.cFileName << "\n";
		if (std::string(file.cFileName) != "..")
			if ((file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				//std::string newPath = CreatePath(file.cFileName, dirPath) + "/*";
				std::string newPath = dirPath == "" ? std::string(file.cFileName) + "/*" : dirPath + "\\" + file.cFileName + "/*";
				std::string newDirPath = dirPath == "" ? file.cFileName : dirPath + "\\" + file.cFileName;
				ListFiles(newPath, newDirPath);
			}
			else
			{
				CreateKey(hkey, file.cFileName);
			}
	}

	FindClose(handle);
}

void ListFiles(std::string path = "")
{
	ListFiles(path == "" ? "*" : path + "//*", path);
}

void main()
{
	ListFiles("");
}
