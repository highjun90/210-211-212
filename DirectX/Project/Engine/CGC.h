#pragma once
#include "singleton.h"

class CEntity;

class CGC :
    public CSingleton<CGC>
{
    SINGLE(CGC);
private:
    vector<CEntity*>    m_vecEntity;

public:
    void tick();

    void Add(CEntity* _Entity)
    {
        m_vecEntity.push_back(_Entity);
    }
};

