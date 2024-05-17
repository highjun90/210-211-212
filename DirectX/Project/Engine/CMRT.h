#pragma once
#include "CEntity.h"


class CMRT :
    public CEntity
{
private:
    Ptr<CTexture>           m_arrRTTex[8];
    ID3D11RenderTargetView* m_RTView[8];
    Vec4                    m_ClearColor[8];
    UINT                    m_RTCount;
    Ptr<CTexture>           m_DSTex;
    D3D11_VIEWPORT          m_ViewPort;


public:
    void Create(Ptr<CTexture>* _pArrTex, UINT _RTCount, Ptr<CTexture> _DSTex);
    void SetClearColor(Vec4* _arrClearColor, UINT _RTCount);

    void OMSet();
    void Clear();
    void ClearRT();
    void ClearDS();

    CLONE_DISABLE(CMRT);
public:
    CMRT();
    ~CMRT();
};

