#pragma once
#include "CAsset.h"


class CShader :
    public CAsset
{
protected:
    ComPtr<ID3DBlob>    m_ErrBlob;


public:
    virtual int UpdateData() = 0;


    CLONE_DISABLE(CShader);
public:
    CShader(ASSET_TYPE _Type, bool _Engine = true);
    ~CShader();
};

