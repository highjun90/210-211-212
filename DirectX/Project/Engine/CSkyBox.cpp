#include "pch.h"
#include "CSkyBox.h"

#include "CAssetMgr.h"
#include "CMesh.h"

#include "CTransform.h"

CSkyBox::CSkyBox()
	: CRenderComponent(COMPONENT_TYPE::SKYBOX)
	, m_SkyBoxType(SKYBOX_TYPE::SPHERE)
{
	SetSkyBoxType(m_SkyBoxType);
	SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"SkyBoxMtrl"));
	DrawShadow(false);
}


CSkyBox::~CSkyBox()
{
}

void CSkyBox::SetSkyBoxType(SKYBOX_TYPE _Type)
{
	m_SkyBoxType = _Type;

	if (SKYBOX_TYPE::SPHERE == m_SkyBoxType)
	{
		SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
	}
	else if (SKYBOX_TYPE::CUBE == m_SkyBoxType)
	{
		SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CubeMesh"));
	}
}

void CSkyBox::finaltick()
{

}

void CSkyBox::UpdateData()
{
	Transform()->UpdateData();

	GetMaterial()->SetScalarParam(SCALAR_PARAM::INT_0, (int)m_SkyBoxType);

	if (SKYBOX_TYPE::SPHERE == m_SkyBoxType)
	{	
		GetMaterial()->SetTexParam(TEX_PARAM::TEX_0, m_SphereTex);
	}
	else if (SKYBOX_TYPE::CUBE == m_SkyBoxType)
	{
		GetMaterial()->SetTexParam(TEX_PARAM::TEXCUBE_0, m_CubeTex);
	}
	
	GetMaterial()->UpdateData();
}

void CSkyBox::render()
{
	UpdateData();

	GetMesh()->render();
}