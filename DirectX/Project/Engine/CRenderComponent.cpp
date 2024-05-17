#include "pch.h"
#include "CRenderComponent.h"

#include "CLevelMgr.h"
#include "CLevel.h"


CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
	: CComponent(_Type)
	, m_DrawShadowMap(true)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& _OriginRenderCom)
	: CComponent(_OriginRenderCom)
	, m_Mesh(_OriginRenderCom.m_Mesh)
	, m_SharedMtrl(_OriginRenderCom.m_SharedMtrl)	
{
	// 원본 오브젝트가 공유재질을 참조하고 있고, 현재 사용재질은 공유재질이 아닌경우
	if (_OriginRenderCom.m_SharedMtrl != _OriginRenderCom.m_CurMtrl)
	{
		assert(_OriginRenderCom.m_DynamicMtrl.Get());

		// 복사 렌더 컴포넌트도 별도의 동적재질을 생성한다.
		GetDynamicMaterial();

		// 원본 렌더컴포넌트의 동적재질 값을 현재 생성한 동적재질로 복사한다.
		*m_DynamicMtrl.Get() = *_OriginRenderCom.m_DynamicMtrl.Get();
	}
	else
	{
		m_CurMtrl = m_SharedMtrl;
	}
}

CRenderComponent::~CRenderComponent()
{

}

void CRenderComponent::SetMaterial(Ptr<CMaterial> _Mtrl)
{
	// 재질이 변경되면 기존에 복사본 받아둔 DynamicMaterial 을 삭제한다.
	m_CurMtrl = m_SharedMtrl = _Mtrl;
	m_DynamicMtrl = nullptr;
}


Ptr<CMaterial> CRenderComponent::GetDynamicMaterial()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	if (pCurLevel->GetState() != LEVEL_STATE::PLAY)
		return nullptr;

	// 이미 동적재질을 보유하고 있으면 그걸 준다.
	if (nullptr != m_DynamicMtrl)
		return m_DynamicMtrl;

	// 공유재질이 있다면
	if (nullptr != m_SharedMtrl)
	{
		// 공유재질을 복사해서 동적재질을 만들고 그걸 현재 사용재질로 설정한다.
		m_CurMtrl = m_DynamicMtrl = m_SharedMtrl->Clone();		
		return m_DynamicMtrl;
	}

	return nullptr;
}

void CRenderComponent::RestoreMaterial()
{
	m_CurMtrl = m_SharedMtrl;	
}


void CRenderComponent::SaveToFile(FILE* _File)
{
	SaveAssetRef(m_Mesh, _File);
	SaveAssetRef(m_SharedMtrl, _File);	
}

void CRenderComponent::LoadFromFile(FILE* _File)
{
	LoadAssetRef(m_Mesh, _File);
	LoadAssetRef(m_SharedMtrl, _File);
	
	SetMaterial(m_SharedMtrl);	
}