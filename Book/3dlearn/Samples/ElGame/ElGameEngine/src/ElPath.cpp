#include "ElApplicationPCH.h"
#include "ElPath.h"
#include "shlwapi.h"

#pragma comment(lib, "shlwapi.lib")

#define ElPathSplitToken			'\\'

bool ElPath::isFileExist(LPCSTR path)
{
	return PathFileExists(path);
}

bool ElPath::createDirectory(LPCTSTR path)
{
	return CreateDirectory(path, NULL) != 0;
}

std::string ElPath::getFileNameFromPath(LPCSTR path)
{
	std::string filename("");
	std::string fullpath(path);

	std::string::size_type pos = fullpath.find_last_of(ElPathSplitToken);
	if (pos != std::string::npos)
		filename = fullpath.substr(pos + 1, fullpath.length() - pos);

	return filename;
}

std::string ElPath::getDirFromPath(LPCSTR path)
{
	std::string dir("");
	std::string fullpath(path);

	std::string::size_type pos = fullpath.find_last_of(ElPathSplitToken);
	if (pos != std::string::npos)
		dir = fullpath.substr(0, pos + 1);

	return dir;
}