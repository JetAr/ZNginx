﻿//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 4)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //

#include "Main.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This is where all the cut scenes and action key handling will be done.
// Only 2 of these functions are action key functions, HandleKey() and HandleMaps().
// The rest of these functions are just for the game's story, etc...  You will
// delete most of them when you make your own game/project.  The method of doing a
// cut scene is simple.  We just make a separate function that writes a bunch
// of dialog to the screen, with pauses between each one.  We then can use functions
// to move the players or monsters/npcs which will make it look like an animated cut
// scene.  We do a cut scene when Quinn saves his brother.  Nothing is hard, tricky
// or complicated about these functions below.
//
// This is how the action keys work.  We have an array, m_actionKeys, that stores
// about 1000 slots for action keys.  We then set a certain index into that array
// as a key.  For instance, the kTrendorSceneKey is 5.  When this key is set to something
// other than 0, it means that we have already done the first Trendor cut scene.  That way
// when we walk into our house and talk to Trendor, there isn't always a huge dialog scene.
// However, we don't always need to set the key, like in the case of a shop keeper.  The
// shop keepers key is 555.  That means when ever we talk to an npc, we will first check
// if it has a action key.  If it does, we then have an if statement that handles this key.
// We don't set this index into our action key array because we always want to be able to
// buy from the shops.  So the key is just to tell us what type of npc it is.  It has
// nothing to do with the action key since we never set or check that index.  We just check
// the number of the npc's attached action key.  We will get into shops in the next tutorial.
//
//


//////////////////////////////// CACTION /////////////////////////////////
/////
/////	This is the CAction class constructor
/////
//////////////////////////////// CACTION /////////////////////////////////

CAction::CAction()
{
    // Let's reset all of the action keys in the array to 0
    memset(m_actionKeys, 0, sizeof(m_actionKeys));
}



//////////////////////////// DISPLAY CUT SCENE DIALOG /////////////////////////////
/////
/////	This displays dialog the redraws the screen
/////
//////////////////////////// DISPLAY CUT SCENE DIALOG /////////////////////////////

void DisplayCutSceneDialog(char *szDialog)
{
    // Display the dialog, pause and wait for user input to continue
    g_Map.DisplayDialog(szDialog);

    // Redraw the screen
    g_Map.SetDrawFlag(true);
    g_Map.Draw();
}


///////////////////////////// ADD TRNEDOR TO GROUP //////////////////////////////
/////
/////	This does the cut scene with Trendor and then adds him to our party
/////
///////////////////////////// ADD TRENDOR TO GROUP //////////////////////////////

void AddTrendorToGroup(CNpc *pNpc)
{
    // This is our first cut scene we will go over.  What happens here is that
    // you are talking to a friend named Trendor in your house after talking to
    // the governor, he wants to join your group.  After the initial dialog is
    // all done, we will add him to our group so that he will follow us.  We
    // never actually display his normal Npc player dialog because we will do
    // this custom dialog instead.  Trendor is an Npc, so we need to convert
    // him into a player, then delete the Npc on the map.

    // Now we need to create a new player dynamically, otherwise it will cause memory
    // problems later.  We will free this later in the CPlayer deconstructor.  After
    // creating memory for the player, we set his image, data, etc...

    // Create a player for Trendor and load his image from the bitmap file
    CPlayer *pTrendor = new CPlayer;
    HBITMAP hTrendorImage = g_Buffer.LoadABitmap((LPSTR)kTrendorImage);

    // Initialize and set Trendor's amazing stats
    pTrendor->Init(hTrendorImage, pNpc->GetIndex().x, pNpc->GetIndex().y);
    pTrendor->SetHealthMax(200);
    pTrendor->SetHealth(200);
    pTrendor->SetProtection(25);
    pTrendor->SetStrength(75);
    pTrendor->SetName(pNpc->GetName());

    // Let's add the new Trendor player to our party and delete the Npc from the map
    g_Player.AddPlayerToParty(pTrendor);
    g_Map.DeleteTile(kNpcType, pNpc->GetIndex().x, pNpc->GetIndex().y);
}


//////////////////////////// DO TRENDOR CUT SCENE /////////////////////////////
/////
/////	This is the cut scene for when Quinn meets Trendor for the first time
/////
//////////////////////////// DO TRENDOR CUT SCENE /////////////////////////////

void DoTrendorCutScene()
{
    // This is the first big dialog scene in our game.  When Quinn talks to
    // Trendor for the first time in his house, they talk about the point of
    // the game, to see if they can find a potion to heal Quinn's sick brother.
    // To do a long dialog scene between the 2 characters, we just call the
    // DisplayCutSceneDialog() function with a new dialog passed in.  It's that simple.
    DisplayCutSceneDialog("Trendor: Quinn, how are you doing?");
    DisplayCutSceneDialog("Quinn: Oh... as good as can be expected I guess...");
    DisplayCutSceneDialog("Trendor: Is something wrong?");
    DisplayCutSceneDialog("Quinn: Trendor, I just got back from the doctor and he seems to think that my brother doesn't have very long to live.");
    DisplayCutSceneDialog("Trendor: Oh no...  ... He's been sick for so long now.  How long does he have left?");
    DisplayCutSceneDialog("Quinn: The doctor said no more than a couple weeks.");
    DisplayCutSceneDialog("Trendor: Quinn, we have to do something!");
    DisplayCutSceneDialog("Quinn: There doesn't seem to be anything left to do but hope.");
    DisplayCutSceneDialog("Trendor: No!  Wait!  I remember, there was a legend about a special place in the new world, where an old sage lived.  Apparently he could cure many diseases and sickness with a special potion.");
    DisplayCutSceneDialog("Quinn: The new world?  There is no way we could get there.  No one has been there in over 200 years since the creatures overran the place.");
    DisplayCutSceneDialog("Trendor: But if there really is a sage, we need to find him!");
    DisplayCutSceneDialog("Quinn: Even if we wanted to go to the new world, they have guards around the bridge day and night.");
    DisplayCutSceneDialog("Trendor: Quinn, go and talk to the Governor.  He is the only person who can let you cross over to the new world.");
    DisplayCutSceneDialog("Quinn: I guess, if that is our last option.  I can't just sit here and do nothing....  I will go!");
    DisplayCutSceneDialog("Trendor: Excellent!  Let me know how it goes.  If there's anything I can do, don't hesitate to ask.");
}

//////////////////////////// DO GOVERNOR CUT SCENE /////////////////////////////
/////
/////	This is the cut scene for when Quinn meets the governor for the first time
/////
//////////////////////////// DO GOVERNOR CUT SCENE /////////////////////////////

void DoGovernorCutScene()
{
    // This is the first dialog that we do with the Governor.  This basically sets
    // up the quest of going and getting stronger before the Governor will give you
    // permission to go into the new world to find the sage and get the potion.
    DisplayCutSceneDialog("Quinn: Hello Governor.  I'm Quinn.  I have a very important matter to discuss with you.");
    DisplayCutSceneDialog("Governor: How can I help you young man?");
    DisplayCutSceneDialog("Quinn: You see, my brother is very sick... and I need to cross over into the new world to find a sage that can help my brother.");
    DisplayCutSceneDialog("Governor: Cross over into the new world?  Are you smoking the terran weed young man?  Do you know how dangerous that place has become?");
    DisplayCutSceneDialog("Quinn: I know, but It's a risk I am willing to take.");
    DisplayCutSceneDialog("Governor: No one has ventured into that land in many years.");
    DisplayCutSceneDialog("Quinn: Yes sir, but I apparently need your permission to do so.  It's a matter of life and death.");
    DisplayCutSceneDialog("Governor: Your death I can promise you that.");
    DisplayCutSceneDialog("Quinn: Please Governor.  I will do anything.  Just give me permission and I will be on my way.");
    DisplayCutSceneDialog("Governor: Hmmm, ... ... You must really love your brother.  I will tell you what, you don't look like you could handle the creatures in the new world right now.");
    DisplayCutSceneDialog("Quinn: I am confident I can get by.");
    DisplayCutSceneDialog("Governor: No no, in order to get my permission you must get stronger and have more experience.");
    DisplayCutSceneDialog("Quinn: What?");
    DisplayCutSceneDialog("Governor: Go out and get more fighting experience with some creates and come back when you are a bit stronger.");
    DisplayCutSceneDialog("Quinn: Okay, I will be back.");
}


//////////////////////////// DO SAGE CUT SCENE /////////////////////////////
/////
/////	This is the cut scene for when Quinn meets the sage for the first time
/////
//////////////////////////// DO SAGE CUT SCENE /////////////////////////////

void DoSageCutScene()
{
    // This is the dialog scene that the player has with the sage.  This is
    // where Quinn gets the potion to save his brother.
    DisplayCutSceneDialog("Quinn: Are you the legendary sage?");
    DisplayCutSceneDialog("Sage: Well, that's the best introduction I have ever had!");
    DisplayCutSceneDialog("Quinn: I have traveled for a long time to find you.  You see, my brother is sick and needs help.");
    DisplayCutSceneDialog("Sage: Well you came to the right place!  I happen to have a few potions sitting around here.");
    DisplayCutSceneDialog("Quinn: Wonderful, I need one.  Please give it to me!");
    DisplayCutSceneDialog("Sage: Wait just one second, usually I charge about $2500 for each potion.");
    DisplayCutSceneDialog("Quinn: Oh no.... I don't have that kind of money...");
    DisplayCutSceneDialog("Sage: Hmm, well an old sage has to make a living.");
    DisplayCutSceneDialog("Quinn: Please sir, is there anyway you can make an exception just this once?");
    DisplayCutSceneDialog("Sage: Well, you seem like a nice young man.  I will give you a potion since you had enough faith to travel here, even though you didn't know if I really lived.");
    DisplayCutSceneDialog("Quinn: Oh, thank you so much!");
    DisplayCutSceneDialog("Sage: Here you go, go save your brother.");
}


//////////////////////////// DO END CUT SCENE /////////////////////////////
/////
/////	This is the final cut scene where Quinn brings his brother the potion
/////
//////////////////////////// DO END CUT SCENE /////////////////////////////

void DoEndCutScene()
{
    // This cut scene moves the player slowly to his brother and gives
    // him the potion, with a small dialog scene.  Then game over!

    // Redraw the screen and do a small pause
    g_Map.SetDrawFlag(true);
    g_Map.Draw();
    g_Buffer.SwapBackBuffer(FALSE);
    Sleep(1000);

    // Here we make the dramatic movement of the player to his brother.
    // First we move the player up 5 spaces.  Each movement we redraw.
    for(int i = 0; i < 5; i++)
    {
        g_Player.Move(kUp);
        g_Map.SetDrawFlag(true);
        g_Map.Draw();
        g_Buffer.SwapBackBuffer(FALSE);
        Sleep(500);
    }

    // Now we move the player over to his brother (left 7 spaces)
    for(int i = 0; i < 7; i++)
    {
        g_Player.Move(kLeft);
        g_Map.SetDrawFlag(true);
        g_Map.Draw();
        g_Buffer.SwapBackBuffer(FALSE);
        Sleep(500);
    }

    // Now do the hero dialog scene
    DisplayCutSceneDialog("Quinn: Hello brother, I'm back.");
    DisplayCutSceneDialog("Brother: Quinn, you're home... ... I don't think I am going to make it...");
    DisplayCutSceneDialog("Quinn: Don't worry about that.  Hold on... ... take this.");

    // Tell the user that we gave the potion (but we really don't! Sneaky huh!?!?)
    g_Menu.DrawMessageBox("Quinn gives his brother the potion.");
    Sleep(3000);

    // The brother is cured!
    DisplayCutSceneDialog("Brother: Wow, ... that tasted horrible!");
    DisplayCutSceneDialog("Quinn: *laugh* ... I'm glad to hear that you still have your humor.");
    DisplayCutSceneDialog("Brother: I feel better already... thanks Quinn!");


    // Now we put the game over message on the screen, then quit the game
    g_Menu.DrawMessageBox("You saved the brother!  *** Game Over ***");
    Sleep(5000);
    PostQuitMessage(0);
}

/////////////////////////// GIVE POTION ////////////////////////////
/////
/////	This creates the potion item, then gives it to the player
/////
/////////////////////////// GIVE POTION ////////////////////////////

void GivePotion()
{
    // Create a new potion item and give it it's appropriate image
    CItem potion;
    HBITMAP hPotionImage = g_Buffer.LoadABitmap((LPSTR)kPotionImage);

    // Intialize the potion with an action key and image.  Also set the
    // event of receiving the potion to a 1 in the global action keys array.
    g_ActionKeys.SetActionKey(kPotionKey,1);
    potion.SetActionKey(kPotionKey);
    potion.SetBitmap(hPotionImage);

    // Add the potion to the player's inventory and post a message about it
    g_Player.AddItem(potion);
    g_Menu.DrawMessageBox("You received the potion!");
    Sleep(1000);
}


//////////////////////////////// HANDLE KEY /////////////////////////////////
/////
/////	This function takes an action key and a tile type to handle
/////
//////////////////////////////// HANDLE KEY /////////////////////////////////

void CAction::HandleKey(int actionKey, CTile *pTile)
{
    // This is our main action key function.  This will take an action key
    // so we know what should happen, and then a tile, which is default to NULL.
    // We don't always need a tile (which can be casted to a npc, exit, etc...),
    // but it can be necessary sometimes for your game situations.

    // If the key is the shop keeper key
    if(actionKey == kShopKeeperKey)
    {
        // We don't handle shops yet
    }
    // If we are doing the first contact with Trendor
    else if(actionKey == kTrendorSceneKey)
    {
        // If we have not already done this, do the first dialog scene
        if(m_actionKeys[actionKey] != 1)
            DoTrendorCutScene();
        // Otherwise, add Trendor to our party if we already talked to the Governor
        else if(m_actionKeys[kGovernorSceneKey])
            HandleKey(kAddTrendorKey, pTile);
        else // Else encourage the player to talk to the Governor
            DisplayCutSceneDialog("Trendor: Quinn, let me know if there is anything I can do to help.  I am interested in what the Governor will say.");

        // Set this action key as "happened" (1)
        m_actionKeys[actionKey] = 1;
    }
    // If the key is to add Trendor
    else if(actionKey == kAddTrendorKey)
    {
        // Check if we haven't added Trendor yet, then add him
        if(!m_actionKeys[actionKey])
        {
            DisplayCutSceneDialog("Trendor: You talked to the Governor?!?!?  That's great.  I am coming with you.  I don't want to just sit here and do nothing too.");

            // Cast the passed in tile to it's correct type, which is an npc, then add Trendor
            CNpc *pNpc = (CNpc*)pTile;
            AddTrendorToGroup(pNpc);
            m_actionKeys[actionKey] = 1;

            g_Menu.DrawMessageBox("Trendor Joined your party!");
        }
    }
    // If the action key is that we found the potion
    else if(actionKey == kPotionKey)
    {
        // Set the potion key to being accomplished
        m_actionKeys[actionKey] = 1;
    }
    // If we are meeting the Governor
    else if(actionKey == kGovernorSceneKey)
    {
        // If we haven't talked with the Governor yet, do the long dialog
        if(m_actionKeys[actionKey] != 1)
            DoGovernorCutScene();
        else	// Otherwise, see if the player needs permission and is good enough
        {
            // If the player has enough experience to get permission
            if(g_Player.GetExperience() >= kPermissionExpNeeded)
            {
                // If we already received permission, wish the player good luck
                if(m_actionKeys[kPermissionKey])
                    DisplayCutSceneDialog("Governor: Good luck in the new world.  I hope you find the sage you seek.");
                else
                {
                    // Otherwise, let's give the player permission to go across the bridge
                    DisplayCutSceneDialog("Governor: Wow Quinn, you have really improved.  I think with your experience and strength you should be fine.  You have my permission to enter the new world.");
                    m_actionKeys[kPermissionKey] = 1;
                }
            }
            else	// We aren't experienced enough, give encouragement
                DisplayCutSceneDialog("Governor: I think you need to be a little stronger and with more experience.  Come back after you improve a bit more.");
        }

        // Set the action key to true so that we know we already talked with the Governor
        m_actionKeys[actionKey] = 1;
    }
    // If we are talking with the bridge guard
    else if(actionKey == kBridgeGuardKey)
    {
        // If we don't have permission, tell the player they need permission to pass
        if(m_actionKeys[kPermissionKey] == 0)
            DisplayCutSceneDialog("Guard: Nobody gets by me unless you have the Governor's permission.");
        else	// Otherwise, we got permission!
        {
            // Cast the tile as the guard npc
            CNpc *pGuard = (CNpc*)pTile;

            // If the player is on the left side of the bridge, say we received permission
            if(g_Player.GetPosition().x <= pGuard->GetIndex().x)
            {
                // Move the player past the guard and safely across the bridge
                POINT newPosition = {g_Player.GetPosition().x + 2, g_Player.GetPosition().y};

                // Tell the guard we received permission from the Governor and set new position
                DisplayCutSceneDialog("Quinn: I just received permission from the Governor to pass.");
                DisplayCutSceneDialog("Guard: Okay, you may pass.  Be carefull over there.");
                g_Player.SetPosition(newPosition);
            }
            else	// Otherwise we are coming back from the new world from the right side
            {
                // Move the player passed the guard again, leaving the new world
                POINT newPosition = {g_Player.GetPosition().x - 2, g_Player.GetPosition().y};

                DisplayCutSceneDialog("Guard: Welcome back!  It's good to see you still alive and well.");
                g_Player.SetPosition(newPosition);
            }
        }

        // Set the key s we know we talked to the guard
        m_actionKeys[actionKey] = 1;
    }
    // If we are talking to the sage
    else if(actionKey == kSageSceneKey)
    {
        // If we have already received the potion, tell the player to hurry back home
        if(m_actionKeys[actionKey] != 0)
            DisplayCutSceneDialog("Sage: Hurry up and go save your brother!");
        else
        {
            // Do the first dialog scene with the sage and get the potion from him
            DoSageCutScene();
            GivePotion();
        }

        // Set the key so we know we have already talked with the sage
        m_actionKeys[actionKey] = 1;
    }
    else	// Otherwise set the key as done for whatever key was passed in
        m_actionKeys[actionKey] = 1;
}


//////////////////////////////// HANDLE MAPS /////////////////////////////////
/////
/////	This function handles special events for certain maps upon opening
/////
//////////////////////////////// HANDLE MAPS /////////////////////////////////

void CAction::HandleMaps()
{
    // Every time a map loads, this function is called to see if we need to
    // do a special event, like in the case of the end scene when we save
    // our brother.  You can also use this function to start a new MIDI
    // file for a new map that needs a new song.  Another important thing we
    // do is get rid of the Trendor npc once he has joined our party.  Trendor
    // will join our party, and if we go back into the house, we don't want to
    // see the npc "Trendor" sitting in inside, since he is already attached
    // to us in our party, so we check for this and then delete that npc if
    // Trendor has already joined our party.  This is another way action keys
    // come into play.

    // Get the current map name
    string strMap = g_Map.GetMapName();

    // Make sure we delete items if they are already taken
    g_Map.DeleteTakenItems();

    // If the current map is the house of Quinn
    if(strstr(strMap.c_str(), "House.map"))
    {
        // If we already added Trendor to our party, delete his npc from the map
        if(m_actionKeys[kAddTrendorKey])
        {
            g_Map.DeleteNpc("Trendor");
        }

        // If we received the potion, do the final cut scene to save the brother
        if(m_actionKeys[kPotionKey])
        {
            DoEndCutScene();
        }
    }
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// We implemented the functions necessary to have cut scenes and handle
// special events using action keys.  The important functions to focus
// on are HandleKey() and HandleMaps().  The rest of the functions are
// just extra function for this game that do cut scenes and special
// events.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials
