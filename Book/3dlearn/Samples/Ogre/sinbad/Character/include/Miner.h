#ifndef MINER_H
#define MINER_H
//------------------------------------------------------------------------
//
//  Name:   Miner.h
//
//  Desc:   A class defining a goldminer.
//
//  Author: Mat Buckland 2002 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <string>
#include <cassert>

#include "Locations.h"
#include "MinerOwnedStates.h"
#include <Common\fsm\StateMachine.h>
//#include "2D\Vector2D.h"

#include "Ogre.h"
using namespace Ogre;



//the amount of gold a miner must have before he feels comfortable
const int ComfortLevel       = 500;
//the amount of nuggets a miner can carry
const int MaxNuggets         = 300;
//above this value a miner is thirsty
const int ThirstLevel        = 500;
//above this value a miner is sleepy
const int TirednessThreshold = 500;

class GameMap;

class Miner/* : public BaseGameEntity*/
{
public:

	//an instance of the state machine class
	StateMachine<Miner>*  m_pStateMachine;

	location_type         m_Location;

	//how many nuggets the miner has in his pockets
	int                   m_iGoldCarried;

	int                   m_iMoneyInBank;

	//the higher the value, the thirstier the miner
	int                   m_iThirst;

	//the higher the value, the more tired the miner
	int                   m_iFatigue;

public:

	Miner()
		: m_Location(shack),
		m_iGoldCarried(0),
		m_iMoneyInBank(0),
		m_iThirst(0),
		m_iFatigue(0)
	{
		m_pStateMachine = new StateMachine<Miner>(this);

		m_pStateMachine->SetCurrentState(GoHomeAndSleepTilRested::Instance());

	}

	~Miner(){delete m_pStateMachine;}

	//this must be implemented
	void UpdateState(Real deltaTime);

	StateMachine<Miner>*  GetFSM()const{return m_pStateMachine;}


	location_type Location()const{return m_Location;}
	void          ChangeLocation(const location_type loc){m_Location=loc;}

	int           GoldCarried()const{return m_iGoldCarried;}
	void          SetGoldCarried(const int val){m_iGoldCarried = val;}
	void          AddToGoldCarried(const int val);
	bool          PocketsFull()const{return m_iGoldCarried >= MaxNuggets;}

	bool          Fatigued()const;
	void          DecreaseFatigue(){m_iFatigue -= 1;}
	void          IncreaseFatigue(){m_iFatigue += 1;}

	int           Wealth()const{return m_iMoneyInBank;}
	void          SetWealth(const int val){m_iMoneyInBank = val;}
	void          AddToWealth(const int val);

	bool          Thirsty()const; 
	void          BuyAndDrinkAWhiskey(){m_iThirst = 0; m_iMoneyInBank-=2;}

	void Render();


	//! 是否在当前状态位置
	virtual bool IsPostion() = 0 ;
	virtual void move() = 0;


	void SetMapIniPos(GameMap* pMap);

	Ogre::Vector3 m_curPos;

	//! 废弃
	int x;
	int y;
	int m_width;
	int m_height;
	int           ID()const{return 0;}  

};


#endif
