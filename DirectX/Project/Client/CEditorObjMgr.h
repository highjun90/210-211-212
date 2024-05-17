#pragma once
#include <Engine\singleton.h>

class CGameObjectEx;

class CEditorObjMgr :
    public CSingleton<CEditorObjMgr>
{
    SINGLE(CEditorObjMgr);
private:
    vector<CGameObjectEx*>    m_vecEditorObj;

public:
    void init();
    void progress();
};

