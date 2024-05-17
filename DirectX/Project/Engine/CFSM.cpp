#include "pch.h"
#include "CFSM.h"

CFSM::CFSM(bool _bEngine)
	: CAsset(ASSET_TYPE::FSM, _bEngine)
	, m_Master(nullptr)
	, m_Blackboard(nullptr)	
{
}

CFSM::~CFSM()
{
	if (m_Blackboard)
		delete m_Blackboard;

	if (!m_Master)
	{
		Delete_Map(m_mapState);
	}
}

void CFSM::finaltick()
{
	if (m_CurState)
	{
		m_CurState->m_FSM = this;
		m_CurState->finaltick();
	}
}

void CFSM::AddState(const wstring& _StateName, CState* _State)
{
	assert(!(FindState(_StateName)));

	_State->m_FSM = this;
	m_mapState.insert(make_pair(_StateName, _State));
}

CState* CFSM::FindState(const wstring& _StateName)
{
	map<wstring, CState*>::iterator iter = m_mapState.find(_StateName);

	if (iter == m_mapState.end())
		return nullptr;

	return iter->second;
}

void CFSM::SetState(const wstring& _strState)
{
	m_CurState = FindState(_strState);
}

CFSM* CFSM::GetFSMIstance()
{
	CFSM* pFSMInst = new CFSM;

	pFSMInst->m_mapState = m_mapState;
	pFSMInst->m_Master = this;
	pFSMInst->m_Blackboard = nullptr;
	pFSMInst->m_CurState = nullptr;

	return pFSMInst;
}

void CFSM::ChangeState(const wstring& _strStateName)
{
	if(nullptr != m_CurState)
		m_CurState->Exit();

	m_CurState = FindState(_strStateName);

	assert(m_CurState);

	m_CurState->Enter();
}
