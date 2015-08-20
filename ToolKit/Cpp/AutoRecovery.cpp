/*
    2015-08-20 11:01 BG57IV3
*/
template <class T> 
class CAutoRecovery
{
private:
	T* m_pTarget;
	T m_oFinalVal;

private:
	CAutoRecovery(CAutoRecovery const&);
	CAutoRecovery& operator=(CAutoRecovery const&);

public:
	explicit CAutoRecovery(T* pTarget,const T& rNowVal,const T& rFinalVal)
	{
		m_pTarget = pTarget;
		*m_pTarget = rNowVal;
		m_oFinalVal = rFinalVal;
	}

	~CAutoRecovery()
	{
		*m_pTarget = m_oFinalVal;
		m_pTarget = NULL;
	}
};
