#pragma once
#include "CRenderComponent.h"
class CDecal :
    public CRenderComponent
{
private:
    int    m_bAsEmissive;

public:
    void SetDecalAsEmissive(bool _bTrue) { m_bAsEmissive = _bTrue; }

public:
    virtual void finaltick() override;
    virtual void UpdateData() override;
    virtual void render() override;
        
public:
    CLONE(CDecal);
    CDecal();
    ~CDecal();
};

