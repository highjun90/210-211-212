#pragma once
#include "CComponent.h"

#include "CFSM.h"
#include "CBlackboard.h"

class CStateMachine :
    public CComponent
{
private:
    Ptr<CFSM>       m_FSM;
    Ptr<CFSM>       m_FSM_Inst;

    CBlackboard     m_Blackboard;

public:
    void SetFSM(Ptr<CFSM> _FSM);
    Ptr<CFSM> GetFSM() 
    { 
        if (m_FSM_Inst.Get())
            return m_FSM_Inst;
        else
            m_FSM;
    };

    void AddBlackboardData(const wstring& _strKey, BB_DATA _Type, void* _pData);
    void* GetBlackboardData(const wstring& _strKey);


public:
    virtual void begin() override;
    virtual void finaltick() override;

    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;
public:
    CLONE(CStateMachine);
    CStateMachine();

    ~CStateMachine();
};

