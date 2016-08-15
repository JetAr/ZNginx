#include "GameMap.h"

#include <Common\misc\Stream_Utility_Functions.h>

GameMap::GameMap()
{
	//shack,
	//goldmine,
	//bank,
	//saloon
	LocPos[shack] = Vector3(-50 ,0, -50);
	LocPos[goldmine] = Vector3(50,0 , -50);
	LocPos[bank] = Vector3(-50,0 , 50);
	LocPos[saloon] = Vector3(50,0 , 50);
}

GameMap::~GameMap(void)
{
}


void GameMap::Draw()
{
	//draw the head
	//gdi->BrownBrush();
	//gdi->Circle(Pos(), 6);
	//draw the grass
	//gdi->BlackBrush();
	////gdi->DarkGreenBrush();
	//gdi->Rect(0,0,m_width, m_height);

	//DrawBuilding(shack );
	//DrawBuilding(goldmine);
	//DrawBuilding(bank);
	//DrawBuilding(saloon);

	//game_draw_map();
}

//void GameMap::DrawBuilding(int nTyep)
//{
//	 int w  = m_width;
//	 int h = m_height;
//
//	
//	gdi->TransparentText();
//	gdi->TextColor(255, 255, 0);
//	gdi->WhitePen();
//	gdi->GreenBrush();
//
//	Vector2D v2d = LocPos[(location_type)nTyep];
//	gdi->Circle(v2d , 10 );
//	gdi->Cross( v2d , 10 );
//
//	switch(nTyep)
//	{
//	case shack:
//		{
//			gdi->TextAtPos(v2d.x+10,v2d.y, "Ä¾ÎÝ");
//		}
//		break;
//	case goldmine:
//		{
//			gdi->TextAtPos( v2d.x+10,v2d.y , "½ð¿ó");
//
//		}
//		break;
//	case bank:
//		{
//			gdi->TextAtPos( v2d.x+10,v2d.y, "ÒøÐÐ");
//		}
//		break;
//	case saloon:
//		{
//			gdi->TextAtPos(v2d.x+10,v2d.y, "¾Æ°É");
//
//		}
//		break;
//	}
//
//}


const Vector3& GameMap::GetLocPos(location_type type) const
{
	LOCMAP::const_iterator ent = LocPos.find(type);

	//assert that the entity is a member of the map
	assert ( (ent !=  LocPos.end()) && "<EntityManager::GetEntityFromID>: invalid ID");

	return ent->second;

}
