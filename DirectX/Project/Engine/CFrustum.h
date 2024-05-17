#pragma once
#include "CEntity.h"

enum class FACE
{
    F_NEAR,
    F_FAR,
    F_TOP,
    F_BOTTOM,
    F_LEFT,
    F_RIGHT,
    END,
};


class CCamera;

class CFrustum :
    public CEntity
{
private:
    CCamera*    m_Owner;
    Vec3        m_ProjPos[8];
    Vec4        m_arrFace[(UINT)FACE::END];    



public:
    bool FrustumCheck(Vec3 _vWorldPos);


public:
    void finaltick();

private:
    void SetOwner(CCamera* _Cam) { m_Owner = _Cam; }

public:
    CLONE(CFrustum);
    CFrustum();
    ~CFrustum();

    friend class CCamera;
};

