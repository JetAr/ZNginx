#include "Miner.h"
//#include "misc/ConsoleUtils.h"
//#include <misc\Cgdi.h>



#include <sstream>
using namespace std;

#include "GameMap.h"

//-----------------------------------------------------------------------------
void Miner::UpdateState(Real deltaTime)
{
  //SetTextColor(FOREGROUND_RED| FOREGROUND_INTENSITY);
	//static Real timeDelay = deltaTime;
	//if (timeDelay>0.1)
	//{
	//	if (m_pStateMachine->CurrentState() != BeMovingState::Instance())
	//	{
	//		m_iThirst += 1;
	//	}

	//	m_pStateMachine->Update();

	//	timeDelay = 0;
	//}
	//else
	//{
	//	timeDelay+=deltaTime;
	//}

	if (m_pStateMachine->CurrentState() != BeMovingState::Instance())
	{
		m_iThirst += 1;
	}

	m_pStateMachine->Update();


}

//-----------------------------------------------------------------------------
void Miner::AddToGoldCarried(const int val)
{
  m_iGoldCarried += val;

  if (m_iGoldCarried < 0) m_iGoldCarried = 0;
}

//-----------------------------------------------------------------------------
void Miner::AddToWealth(const int val)
{
  m_iMoneyInBank += val;

  if (m_iMoneyInBank < 0) m_iMoneyInBank = 0;
}

//-----------------------------------------------------------------------------
bool Miner::Thirsty()const
{
  if (m_iThirst >= ThirstLevel){return true;}

  return false;
}


//-----------------------------------------------------------------------------
bool Miner::Fatigued()const
{
  if (m_iFatigue > TirednessThreshold)
  {
    return true;
  }

  return false;
}


void Miner::Render()
{

	//gdi->Rect( 10 , 10 , 90 , 90);
	switch(m_Location)
	{
	case shack:
		{
			x = 20  ;
			y = 20;
		}
		break;
	case goldmine:
		{
			x = m_width - 80 ;
			y = 60;

		}
		break;
	case bank:
		{
			x = 50 ;
			y = m_height - 60;
		}
		break;
	case saloon:
		{
			x = m_width- 80 ;
			y = m_height - 60;

		}
		break;
	}

	//gdi->GreenPen();
	//gdi->GreenBrush();
	//gdi->TextColor(0, 0, 255 );
	//gdi->TextAtPos(m_curPos.x,m_curPos.y, "<ฟ๓นค>");

	//gdi->RedBrush();


	//gdi->TextColor(128, 128, 128 );
	//string inf;
	std::ostringstream   inf;
	inf<<"miner ";
	inf<<"gold:"<<m_iGoldCarried<<" ";
	inf<<"money:"<<m_iMoneyInBank<<" ";
	inf<<"thirst:"<<m_iThirst<<" ";
	inf<<"fatigue:"<<m_iFatigue<<" ";
	//gdi->TextAtPos(m_width/2 - 150 , m_height/2, inf.str());


	//int                   m_iGoldCarried;

	//int                   m_iMoneyInBank;

	////the higher the value, the thirstier the miner
	//int                   m_iThirst;

	////the higher the value, the more tired the miner
	//int                   m_iFatigue;



}

void Miner::SetMapIniPos(GameMap* pMap)
{
	//assert(pMap&&"map is not exist");

	//m_curPos = m_map->GetLocPos(m_Location);


}


//bool Miner::IsPostion()
//{
//	Vector3 vDestPos = GameMap::Instance()->GetLocPos(m_Location);
//	double distance = m_curPos.distance(vDestPos);
//
//	return distance<1;
//
//}

//void Miner::Move()
//{
//	//Vector2D vDestPos = m_map->GetLocPos(m_Location);
//
//	//Vector2D v = vDestPos - m_curPos;
//	//
//	//v.Truncate(1);
//
//
//	//m_curPos+=v;
//
//}
