#ifndef __ElPath_H__
#define __ElPath_H__

#include <string>

class ElPath
{
public:
	static bool isFileExist(LPCSTR path);
	static bool createDirectory(LPCTSTR path);
	static std::string getFileNameFromPath(LPCSTR path);
	static std::string getDirFromPath(LPCSTR path);
};

#endif //__ElPath_H__