#pragma once

class CTimeMgr
	: public CSingleton<CTimeMgr>
{
	SINGLE(CTimeMgr);
private:
	// Larget Integer 는 그냥 8바이트 long long 타입 정수로 취급
	LARGE_INTEGER	m_Frequency;
	LARGE_INTEGER	m_PrevCount;
	LARGE_INTEGER	m_CurCount;

	double			m_DeltaTime;
	double			m_EngineDeltaTime;

	UINT			m_iCall;
	double			m_Time;
	bool			m_bLock;

	wchar_t			m_szText[256];

public:
	float GetDeltaTime() { return (float)m_DeltaTime; }
	double GetDeltaTime_d() { return m_DeltaTime; }

	float GetEngineDeltaTime() { return (float)m_EngineDeltaTime; }
	double GetEngineDeltaTime_d() { return m_EngineDeltaTime; }

	void LockDeltaTime(bool _Lock) { m_bLock = _Lock; }

public:
	void init();
	void tick();
	void render();
};

