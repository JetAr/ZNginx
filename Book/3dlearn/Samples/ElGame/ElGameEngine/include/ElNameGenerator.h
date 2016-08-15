#ifndef __ElNameGenerator_H__
#define __ElNameGenerator_H__

#include "ElSingleton.h"
#include <string>

class ElNameGenerator
{
	ElDeclareSingleton(ElNameGenerator);

public:
	ElNameGenerator();
	virtual ~ElNameGenerator();

	std::string generateNodeName();
	std::string generateMovableObjectName();

protected:
	int		mNodeUniqueID;
	int		mMovableObjectUniqueID;
};

#endif //__ElNameGenerator_H__