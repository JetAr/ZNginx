﻿#ifndef _PLAYER_H
#define _PLAYER_H

// Include the main header file
#include "main.h"

// This is our starting position for our player on the map
const int kPlayerStartX = 25;
const int kPlayerStartY = 25;

// This is the direction constants that we use for moving the character
enum eDirections
{
    kUp = 0,
    kDown,
    kLeft,
    kRight
};


// This is our player class
class CPlayer
{
public:

    // The constuctor and deconstructor
    CPlayer();
    ~CPlayer();

    // This inits the player with an image and a position
    void Init(CHAR_INFO image, int x, int y);

    // This draws the player to the screen
    void Draw();

    // This moves the player according to the desired direction (kUp, kDown, kLeft, kRight)
    bool Move(int direction);

    // This restores the players previous position
    void MovePlayerBack()
    {
        m_position = m_lastPosition;
    }

    // These functions set the player's data
    void SetName(char *szPlayerName)
    {
        strcpy(m_szName, szPlayerName);
    }
    void SetHealth(int playerHealth)
    {
        m_health = playerHealth;
    }
    void SetHealthMax(int maxHealth)
    {
        m_healthMax = maxHealth;
    }
    void SetStrength(int playerStrength)
    {
        m_strength = playerStrength;
    }
    void SetProtection(int protection)
    {
        m_protection = protection;
    }
    void SetExperience(int experience)
    {
        m_experience = experience;
    }
    void SetLevel(int level)
    {
        m_level = level;
    }
    void SetGold(int gold)
    {
        m_gold = gold;
    }

    // These function retrieve info about the player
    int GetStrength()
    {
        return m_strength;
    }
    int GetHealth()
    {
        return m_health;
    }
    int GetHealthMax()
    {
        return m_healthMax;
    }
    int GetProtection()
    {
        return m_protection;
    }
    int GetExperience()
    {
        return m_experience;
    }
    int GetLevel()
    {
        return m_level;
    }
    int GetGold()
    {
        return m_gold;
    }
    char *GetName()
    {
        return m_szName;
    }

    // These get and set the player's image
    CHAR_INFO GetImage()
    {
        return m_image;
    }
    void SetImage(CHAR_INFO newImage)
    {
        m_image = newImage;
    }

    // These get and set the players position
    COORD GetPosition()
    {
        return m_position;
    }
    void SetPosition(COORD newPosition)
    {
        m_position = newPosition;
    }

    // These get and set the players last position
    COORD GetLastPosition()
    {
        return m_lastPosition;
    }
    void SetLastPosition(COORD lastPosition)
    {
        m_lastPosition = lastPosition;
    }

    // This tells us if the player is still alive or not
    bool IsAlive()
    {
        return (m_health > 0);
    }

private:
    CHAR_INFO m_image;				// The player's image
    COORD m_position;				// The player's position
    COORD m_lastPosition;			// The player's last position
    char m_szName[kMaxNameLen];		// The player's name
    int m_health;					// The player's current health
    int m_strength;					// The player's strength
    int m_protection;				// The player's protection
    int m_experience;				// The player's experience points
    int m_level;					// The player's level
    int m_gold;						// The player's gold

    int m_healthMax;				// The player's max health

    // These below are currently not all being used, but we will include them in the class

    CItem *m_pHead;					// The current equipment on the player's head
    CItem *m_pChest;				// The current equipment on the player's chest
    CItem *m_pWeapon;				// The current player's weapon
    CItem *m_pFeet;					// The current equipment on the player's feet

    vector<CPlayer*> m_vParty;		// The list of players in your party
    vector<CItem> m_vItems;			// The inventory list for the player
};


#endif


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// Nothing new was added to this file for this tutorial.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials
