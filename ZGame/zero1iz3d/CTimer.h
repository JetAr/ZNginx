// CTimer.h -

/*

 Timer Classes

 by Peter Petrov
  
  - History	
     30.11.2003 - established
	 2?.03.2003 - added CFrame class

*/


#ifndef __CTIMER_INCLUDED
#define __CTIMER_INCLUDED



// timer class
class CTimer
{

private:
	
	int m_starttime;
	int m_pausetime;
	int m_pausestart;


public:
	CTimer() { m_pausetime = m_starttime = m_pausestart = 0; m_paused = false; };

	bool m_paused;

	void Start();
	int Time();
	void Pause();
	void Unpause();
	bool Elapsed( int &time_start, int interval );
};



// Game Frame class
class CGameFrame
{

private:
	CTimer *timer; 
	int    lasttime;
	float  motion_frame;	// motion per frame

	int	   fps;
	int    frames;
	int    frametime;
	
public:

	CGameFrame();
	CGameFrame( CTimer *Timer );

	void setTimer( CTimer *Timer ) { timer = Timer; lasttime = timer->Time(); };
	void Update();
	int getFps() { return fps; };
	float getMpf() { return motion_frame; };

};


#endif