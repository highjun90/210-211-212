#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer :
    public CEntity
{
private:
    vector<CGameObject*>    m_vecParent;
    vector<CGameObject*>    m_vecObjects;
    int                     m_iLayerIdx;

private:
    void begin();
    void tick();
    void finaltick();    

public:
    // 특정 오브젝트를 레이어에서 제거
    void DetachGameObject(CGameObject* _Object);
    void RegisterGameObject(CGameObject* _Object) { m_vecObjects.push_back(_Object); }
    int GetLayerIdx() { return m_iLayerIdx; }

    const vector<CGameObject*>& GetParentObjects() { return  m_vecParent; }
    const vector<CGameObject*>& GetLayerObjects() { return m_vecObjects; }

    void AddObject(CGameObject* _Object, bool _bMove);    
    
public:
    CLONE(CLayer);
    CLayer();
    CLayer(const CLayer& _OriginLayer);
    ~CLayer();

    friend class CLevel;
};

