#include "ElApplicationPCH.h"
#include "ElNameGenerator.h"
#include <sstream>

ElImplementSingleton(ElNameGenerator);

ElNameGenerator::ElNameGenerator()
: mNodeUniqueID(0)
, mMovableObjectUniqueID(0)
{

}

ElNameGenerator::~ElNameGenerator()
{

}

std::string ElNameGenerator::generateNodeName()
{
	std::stringstream ss;
	ss << "__Node_" << mNodeUniqueID++ << "__";
	std::string name = ss.str();

	return name;
}

std::string ElNameGenerator::generateMovableObjectName()
{
	std::stringstream ss;
	ss << "__MovableObject_" << mMovableObjectUniqueID++ << "__";
	std::string name = ss.str();

	return name;
}
