﻿#ifndef _OBJECT_H
#define _OBJECT_H

#include "main.h"

// This is the number of X tiles for our map
const int kMapWidth			= 20;
// This is the number of the Y tiles for our map
const int kMapHeight		= 15;

// This is the width in pixels of each tile
const int  kTileWidth			= 32;
// This is the height in pixels of each tile
const int  kTileHeight			= 32;

// The max characters for names
const int kMaxNameLen		= 50;

// The max message length for NPC's, etc...
const int kMaxMessageLen	= 1000;

// The map/level file extension for our editor
const char kMapExt[]		= ".map";

// This is the first map that we load in at the beginning of the game
const char kStartMap[]		= "blcorner.map";

// We include the text file that holds all the items in the game because we
// will use this when doing shops that the user can buy and sell from.  These are
// not the same items as the ones in the map editor tutorial, but the same format.
const char kItemInfoFile[]	= "Items\\items.txt";


// These are the tile types that we use to know which tile type to cast a tile to
enum eTileTypes
{
    kTileType = 0,
    kItemType,
    kMonsterType,
    kNpcType,
    kExitType
};

// These are the types that a tile can be, in regards to collision, however,
// the door type isn't going to be used in this tutorial series.
enum eTileType
{
    kFloor = 0,
    kWall,
    kDoor
};


// These are the types than an item can be.  If it's not equipment, it's a normal item.
enum eItemTypes
{
    kItem = 0,
    kHead,
    kChest,
    kWeapon,
    kFeet
};


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This is the max characters for each line in the dialog box
const int kDlgLineLength = 54;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


// The directories for our resource files
const char kMapsDir[]		= "Maps\\";
const char kTilesDir[]		= "Tiles\\";
const char kItemsDir[]		= "Items\\";
const char kMonstersDir[]	= "Monsters\\";
const char kNpcsDir[]		= "Npcs\\";

// <<////////////// TILE DEFINES ///////////////////>>

// The normal map tiles have these written in the file name
const char kFloorFile[] =	"_f";
const char kWallFile[] =	"_w";
const char kDoorFile[] = 	"_d";


// Our <Base Class> for each tile type.
// This is then inherited from to create other tile types.
class CTile
{
public:

    CTile();	// The constructor (initializing function)

    // Our standard "Set" functions for our protected data
    void SetBitmap(HBITMAP bmpNew)
    {
        m_bmpTile = bmpNew;
    }
    void SetFileName(char *szNew)
    {
        strcpy(m_szFile, szNew);
    }
    void SetType(char newType)
    {
        m_type = newType;
    }
    void SetIndex(int x, int y)
    {
        m_index.x = x;
        m_index.y = y;
    }

    // Our standard "Get" functions for the protected data
    HBITMAP GetBitmap()
    {
        return m_bmpTile;
    }
    char *GetFileName()
    {
        return m_szFile;
    }
    char GetType()
    {
        return m_type;
    }
    POINT GetIndex()
    {
        return m_index;
    }

    bool IsSolid()
    {
        return m_bSolid;
    }

    // The set function for the tile class
    void SetSolidValue(bool newValue)
    {
        m_bSolid = newValue;
    }
    void SetActionKey(int actionKey)
    {
        m_actionKey = actionKey;
    }
    int GetActionKey()
    {
        return m_actionKey;
    }


// Set it protected so that inherited classes can access if necessary
protected:

    HBITMAP m_bmpTile;				// The handle to the tile's bitmap
    char m_szFile[MAX_PATH];		// The file name of the bitmap
    char m_type;					// The type of the tile
    POINT m_index;					// The index on the map of the tile
    bool m_bSolid;					// This tells us if we should collide with the character
    int m_actionKey;				// This stores the tile's action key for your game event triggers
};


// This class inherits from <Base Class> CTile and stores our item information
class CItem : public CTile
{
public:

    CItem();			// The constructor

    // Our standard "Set" functions for private data
    void SetLifeInc(int newInc)
    {
        m_lifeInc = newInc;
    }
    void SetStrengthInc(int newInc)
    {
        m_strengthInc = newInc;
    }
    void SetProtectionInc(int newInc)
    {
        m_protectionInc = newInc;
    }
    void SetName(char *szNew)
    {
        strcpy(m_szItem, szNew);
    }
    void SetItemType(int type)
    {
        m_itemType = type;
    }

    // Our standard "Get" functions for private data
    int GetItemType()
    {
        return m_itemType;
    }
    int GetLifeInc()
    {
        return m_lifeInc;
    }
    int GetStrengthInc()
    {
        return m_strengthInc;
    }
    int GetProtectionInc()
    {
        return m_protectionInc;
    }
    char *GetItemName()
    {
        return m_szItem;
    }

private:

    int m_itemType;					// This tells us what type this item is
    int m_lifeInc;					// The life increase for item
    int m_strengthInc;				// The strength increase for this item
    int m_protectionInc;			// The protection increase for this item
    char m_szItem[kMaxNameLen];		// The name of the item
};


// This class inherits from <Base Class> CTile and stores our monster information
class CMonster : public CTile
{
public:

    CMonster();		// The constructor

    // Our standard "Set" functions for the private data
    void SetLife(int newLife)
    {
        m_life = newLife;
    }
    void SetStrength(int newStrength)
    {
        m_strength = newStrength;
    }
    void SetName(char *szNew)
    {
        strcpy(m_szMonster, szNew);
    }
    void SetSpeed(int newSpeed)
    {
        m_speed = newSpeed;
    }

    // Our standard "Get" functions for the private data
    int GetLife()
    {
        return m_life;
    }
    int GetStrength()
    {
        return m_strength;
    }
    int GetSpeed()
    {
        return m_speed;
    }
    char *GetName()
    {
        return m_szMonster;
    }

private:

    int m_life;						// The life of the monster
    int m_strength;					// The strength of the monster
    int m_speed;					// The speed of the monster
    char m_szMonster[kMaxNameLen];  // The name of the monster
};


// This class inherits from <Base Class> CTile and stores our Npc information
class CNpc : public CTile
{
public:

    CNpc();			// The constructor

    // Our standard "Set" functions for the private data
    void SetLife(int newLife)
    {
        m_life = newLife;
    }
    void SetStrength(int newStrength)
    {
        m_strength = newStrength;
    }
    void SetName(char *szNew)
    {
        strcpy(m_szName, szNew);
    }
    void SetMessage(char *szNew)
    {
        strncpy(m_szMessage, szNew, kMaxMessageLen);
    }
    void SetSpeed(int newSpeed)
    {
        m_speed = newSpeed;
    }

    // Our standard "Get" functions for the private data
    int GetLife()
    {
        return m_life;
    }
    int GetStrength()
    {
        return m_strength;
    }
    int GetSpeed()
    {
        return m_speed;
    }
    char *GetName()
    {
        return m_szName;
    }
    char *GetMessage()
    {
        return m_szMessage;
    }

private:

    int m_life;						// The life of the Npc
    int m_strength;					// The strength of the Npc
    int m_speed;					// The speed of the Npc
    char m_szName[kMaxNameLen];		// The name of the Npc
    char m_szMessage[kMaxMessageLen];	// The message that the Npc says when you talk to them
};


// This class inherits from <Base Class> CTile and stores our exit information
class CExit : public CTile
{
public:

    CExit();			// The constructor

    // Our standard "Set" functions for the private data
    void SetCurrentMap(char *szCurrent)
    {
        strcpy(m_szCurrentMap, szCurrent);
    }
    void SetNewMap(char *szNew)
    {
        strcpy(m_szNewMap, szNew);
    }
    void SetNewPos(POINT position)
    {
        m_newPosition = position;
    }

    // Our standard "Get" functions for the private data
    char *GetCurrentMap()
    {
        return m_szCurrentMap;
    }
    char *GetNewMap()
    {
        return m_szNewMap;
    }
    POINT GetNewPos()
    {
        return m_newPosition;
    }

private:

    char m_szCurrentMap[MAX_PATH];	// The current map name of this exit
    char m_szNewMap[MAX_PATH];		// The destination map's name
    POINT m_newPosition;			// The new position on the next map to exit to
};


// This class handles most of our map functionality
class CMap
{
public:

    CMap();			// The constructor
    ~CMap();		// The deconstructor

    // This free's the image resources for the map
    void FreeResources();

    // This resets the map to green grass
    void SetDefault();

    // This draws our map
    void Draw();

    // This returns the current map's name
    char *GetMapName()
    {
        return m_szMapName;
    }

    // This loads a file
    void Load(const char *szFile);

    // This inserts a tile from the currently selected tile on the map
    void InsertTile(CTile *pTile, int type, int x, int y);

    // This deletes a tile from the map depending on the x and y position
    void DeleteTile(int type, int mapX, int mapY);

    // This returns if the player collided or not with a solid tile
    bool IsACollision(int x, int y);

    // This checks if the player walked into an exit, then takes the exit if so
    void CheckForExit(int x, int y);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

    // This checks if we ran into an npc and displays the dialog if so
    void CheckForNpc(int x, int y);

    // This displays npc dialog in a typewriter effect to the screen
    void DisplayDialog(char *szDialog);

    // This randomly moves the npcs around the map to simulate simple movement
    void MoveNpcs();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


    // This tells the Draw() function to draw the map again
    void SetDrawFlag(bool bDraw)
    {
        m_bDrawScreen = bDraw;
    }

private:

    char m_szMapName[MAX_PATH];		// The name of the map
    bool m_bDrawScreen;				// The draw screen flag
    bool m_bShowStats;				// This tells us if we need to draw the stats bar or not

    vector<CTile> m_vTiles;			// The normal map tiles list on our map
    vector<CItem> m_vItems;			// The item list for our map
    vector<CMonster> m_vMonsters;	// The monster list for our map
    vector<CNpc>  m_vNpcs;			// The npc list for our map
    vector<CExit> m_vExits;			// The exit list for our map
};


#endif


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// We added 3 new functions to our CMap class which help us handle
// dialog with our npcs as well as move them randomly around the map.
// We added at the top a define to control how long each line of dialog
// can be before we need to wrap around.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials
