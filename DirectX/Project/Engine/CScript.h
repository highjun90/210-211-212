#pragma once
#include "CComponent.h"

#include "CTimeMgr.h"
#include "CKeyMgr.h"
//#include "CResMgr.h"

#include "CGameObject.h"
#include "components.h"

struct tScriptParam
{
    SCRIPT_PARAM    Type;    
    string          Desc;
    void*           pData;
};

class CScript :
    public CComponent
{
private:
    const UINT              m_iScriptType;
    vector<tScriptParam>    m_ScriptParam;


public:
    UINT GetScriptType() { return m_iScriptType; }
    const vector<tScriptParam>& GetScriptParam() { return m_ScriptParam; }


protected:
    void Instantiate(Ptr<CPrefab> _Prefab, Vec3 _vWorldPos, int _LayerIdx);
    void AddScriptParam(SCRIPT_PARAM _Param, const string& _Desc, void* _Data) { m_ScriptParam.push_back(tScriptParam{ _Param , _Desc, _Data }); }

public:
    virtual void finaltick() final {}
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObj, CCollider2D* _OtherCollider) {}
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObj, CCollider2D* _OtherCollider) {}
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObj, CCollider2D* _OtherCollider) {}

    virtual void SaveToFile(FILE* _File) = 0;
    virtual void LoadFromFile(FILE* _File) = 0;



    CScript* Clone() = 0;
public:
    CScript(UINT _ScriptType);
    ~CScript();
};

