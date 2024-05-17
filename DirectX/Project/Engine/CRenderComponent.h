#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

class CRenderComponent :
    public CComponent
{
private:
    Ptr<CMesh>      m_Mesh;

    Ptr<CMaterial>  m_SharedMtrl;
    Ptr<CMaterial>  m_DynamicMtrl;
    Ptr<CMaterial>  m_CurMtrl;

    bool            m_DrawShadowMap;

public:
    void SetMesh(Ptr<CMesh> _Mesh) { m_Mesh = _Mesh; }
    void SetMaterial(Ptr<CMaterial> _Mtrl);
    void DrawShadow(bool _bShadow) { m_DrawShadowMap = _bShadow; }

    Ptr<CMesh> GetMesh() { return m_Mesh; }
    Ptr<CMaterial> GetMaterial() { return m_CurMtrl; }
    Ptr<CMaterial> GetSharedMaterial() { return m_SharedMtrl; }
    Ptr<CMaterial> GetDynamicMaterial();
    bool IsDrawShadow() { return m_DrawShadowMap; }


    void RestoreMaterial();

public:
    virtual void finaltick() {};
    virtual void UpdateData() = 0;
    virtual void render() = 0;


public:
    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;
    virtual CRenderComponent* Clone() = 0;
public:
    CRenderComponent(COMPONENT_TYPE _Type);
    CRenderComponent(const CRenderComponent& _OriginRenderCom);
    ~CRenderComponent();
};

