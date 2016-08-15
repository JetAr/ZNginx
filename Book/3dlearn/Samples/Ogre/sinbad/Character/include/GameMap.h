#pragma once

#include <Ogre.h>
using namespace Ogre;
#include "Locations.h"

#include <map>
//#include <2D\Vector2D.h>

class GameMap
{
	int m_width , m_height;
	GameMap();
	GameMap(const GameMap&);
	GameMap& operator=(const GameMap&);

public:
	~GameMap(void);

	void Draw();

	static GameMap* Instance()
	{
		static GameMap instance;
		return &instance;
	}

	//void DrawBuilding(int nTyep);

	std::map< location_type  , Vector3 > LocPos;
	typedef std::map<location_type  , Vector3 > LOCMAP;


	const Vector3& GetLocPos(location_type) const;
};
