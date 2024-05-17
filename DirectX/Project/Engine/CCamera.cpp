#include "pch.h"
#include "CCamera.h"

#include "CDevice.h"
#include "CTransform.h"

#include "CRenderMgr.h"
#include "CMRT.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CRenderComponent.h"

#include "CAssetMgr.h"
#include "CLight3D.h"


CCamera::CCamera()
	: CComponent(COMPONENT_TYPE::CAMERA)
	, m_ProjType(PROJ_TYPE::ORTHOGRAPHIC)
	, m_FOV(XM_PI / 2.f)
	, m_Width(0.f)
	, m_Scale(1.f)
	, m_AspectRatio(1.f)
	, m_Far(10000.f)
	, m_LayerCheck(0)
	, m_CameraPriority(-1)
{
	Vec2 vResol = CDevice::GetInst()->GetRenderResolution();
	m_Width = vResol.x;
	m_AspectRatio = vResol.x / vResol.y;
}

CCamera::~CCamera()
{
}


void CCamera::begin()
{
	// ī�޶� �켱�������� �°� RenderMgr �� ��Ͻ�Ŵ
	CRenderMgr::GetInst()->RegisterCamera(this, m_CameraPriority);
}

void CCamera::finaltick()
{
	// �� ����� ����Ѵ�.
	// ī�޶� �������� �̵���Ű�� �̵� ���
	Vec3 vCamPos = Transform()->GetRelativePos();
	Matrix matTrans = XMMatrixTranslation(-vCamPos.x, -vCamPos.y, -vCamPos.z);

	// ī�޶��� �� ��, ��, �� �� ������ �������̶� ��ġ��Ű���� ȸ���ϴ� ȸ�����
	Vec3 vRight = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	Vec3 vUp	= Transform()->GetWorldDir(DIR_TYPE::UP);
	Vec3 vFront = Transform()->GetWorldDir(DIR_TYPE::FRONT);

	Matrix matRotate = XMMatrixIdentity();
	matRotate._11 = vRight.x; matRotate._12 = vUp.x; matRotate._13 = vFront.x;
	matRotate._21 = vRight.y; matRotate._22 = vUp.y; matRotate._23 = vFront.y;
	matRotate._31 = vRight.z; matRotate._32 = vUp.z; matRotate._33 = vFront.z;

	// �̵� x ȸ�� = view ���
	m_matView = matTrans * matRotate;
	m_matViewInv = XMMatrixInverse(nullptr, m_matView);


	// ���� ��Ŀ� ���� ���� ����� ����Ѵ�.
	m_matProj = XMMatrixIdentity();

	if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
	{
		// ��������		
		m_matProj = XMMatrixOrthographicLH(m_Width * m_Scale, (m_Width / m_AspectRatio) * m_Scale, 1.f, m_Far);
	}
	else
	{
		// ��������
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, 1.f, m_Far);
	}

	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);	
}

void CCamera::SetCameraPriority(int _Priority)
{
	m_CameraPriority = _Priority;
}



void CCamera::LayerCheck(UINT _LayerIdx, bool _bCheck)
{	
	if (_bCheck)
	{
		m_LayerCheck |= (1 << _LayerIdx);
	}
	else
	{
		m_LayerCheck &= ~(1 << _LayerIdx);
	}
}

void CCamera::LayerCheck(const wstring& _strLayerName, bool _bCheck)
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	CLayer* pLayer = pCurLevel->GetLayer(_strLayerName);

	if (nullptr == pLayer)
		return;

	int idx = pLayer->GetLayerIdx();
	LayerCheck(idx, _bCheck);
}

void CCamera::SortObject()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	for (int i = 0; i < LAYER_MAX; ++i)
	{
		// ī�޶� �ﵵ�� ������ Layer �� �ƴϸ� ����
		if (false == (m_LayerCheck & (1 << i)))
			continue;

		CLayer* pLayer = pCurLevel->GetLayer(i);
		const vector<CGameObject*>& vecObjects = pLayer->GetLayerObjects();
		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			// �޽�, ����, ���̴� Ȯ��
			if (!( vecObjects[j]->GetRenderComopnent()
				&& vecObjects[j]->GetRenderComopnent()->GetMesh().Get()
				&& vecObjects[j]->GetRenderComopnent()->GetMaterial().Get()
				&& vecObjects[j]->GetRenderComopnent()->GetMaterial()->GetShader().Get()))
			{
				continue;
			}

			SHADER_DOMAIN domain = vecObjects[j]->GetRenderComopnent()->GetMaterial()->GetShader()->GetDomain();

			switch (domain)
			{
			case SHADER_DOMAIN::DOMAIN_DEFERRED:
				m_vecDeferred.push_back(vecObjects[j]);
				break;
			case SHADER_DOMAIN::DOMAIN_DECAL:
				m_vecDecal.push_back(vecObjects[j]);
				break;
			case SHADER_DOMAIN::DOMAIN_OPAQUE:
				m_vecOpaque.push_back(vecObjects[j]);
				break;
			case SHADER_DOMAIN::DOMAIN_MASKED:
				m_vecMaked.push_back(vecObjects[j]);
				break;
			case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
				m_vecTransparent.push_back(vecObjects[j]);
				break;
			case SHADER_DOMAIN::DOMAIN_POSTPROCESS:
				m_vecPostProcess.push_back(vecObjects[j]);
				break;
			case SHADER_DOMAIN::DOMAIN_DEBUG:
				break;
			}
		}
	}
}

void CCamera::render_deferred()
{
	for (size_t i = 0; i < m_vecDeferred.size(); ++i)
	{
		m_vecDeferred[i]->render();
	}
	m_vecDeferred.clear();
}


void CCamera::render_decal()
{
	for (size_t i = 0; i < m_vecDecal.size(); ++i)
	{
		m_vecDecal[i]->render();
	}
	m_vecDecal.clear();
}

void CCamera::render_forward()
{
	for (size_t i = 0; i < m_vecOpaque.size(); ++i)
	{
		m_vecOpaque[i]->render();
	}
	m_vecOpaque.clear();

	for (size_t i = 0; i < m_vecMaked.size(); ++i)
	{
		m_vecMaked[i]->render();
	}
	m_vecMaked.clear();

	for (size_t i = 0; i < m_vecTransparent.size(); ++i)
	{
		m_vecTransparent[i]->render();
	}
	m_vecTransparent.clear();
}

void CCamera::render_postprocess()
{
	for (size_t i = 0; i < m_vecPostProcess.size(); ++i)
	{
		// ���� ������ �̹����� ��ó�� Ÿ�ٿ� ����
		CRenderMgr::GetInst()->CopyRenderTargetToPostProcessTarget();

		// ������� ��ó�� �ؽ��ĸ� t13 �������Ϳ� ���ε�
		Ptr<CTexture> pPostProcessTex = CRenderMgr::GetInst()->GetPostProcessTex();
		pPostProcessTex->UpdateData(13);

		// ��ó�� ������Ʈ ������
		m_vecPostProcess[i]->render();
	}

	m_vecPostProcess.clear();
}

void CCamera::Merge()
{
	// Deferred ������ SwapChain ���� ����
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

	static Ptr<CMesh>	   pRectMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
	static Ptr<CMaterial>  pMergeMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"MergeMtrl");
		
	pMergeMtrl->UpdateData();
	pRectMesh->render();
}

void CCamera::SortShadowMapObject()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	for (int i = 0; i < LAYER_MAX; ++i)
	{
		// ī�޶� �ﵵ�� ������ Layer �� �ƴϸ� ����
		if (false == (m_LayerCheck & (1 << i)))
			continue;

		CLayer* pLayer = pCurLevel->GetLayer(i);
		const vector<CGameObject*>& vecObjects = pLayer->GetLayerObjects();
		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			// �޽�, ����, ���̴� Ȯ��
			if (!( vecObjects[j]->Transform()->IsDynamic()
				&& vecObjects[j]->GetRenderComopnent()
				&& vecObjects[j]->GetRenderComopnent()->IsDrawShadow()
				&& vecObjects[j]->GetRenderComopnent()->GetMesh().Get()
				&& vecObjects[j]->GetRenderComopnent()->GetMaterial().Get()
				&& vecObjects[j]->GetRenderComopnent()->GetMaterial()->GetShader().Get()))
			{
				continue;
			}

			m_vecShadow.push_back(vecObjects[j]);
		}
	}
}

void CCamera::render_shadowmap()
{
	Ptr<CMaterial> pShadowMapMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"ShadowMapMtrl");

	g_Transform.matView = m_matView;
	g_Transform.matViewInv = m_matViewInv;
	g_Transform.matProj = m_matProj;
	g_Transform.matProjInv = m_matProjInv;

	for (size_t i = 0; i < m_vecShadow.size(); ++i)
	{
		m_vecShadow[i]->Transform()->UpdateData();
		pShadowMapMtrl->UpdateData();
		m_vecShadow[i]->GetRenderComopnent()->GetMesh()->render();
	}

	m_vecShadow.clear();
}

void CCamera::SaveToFile(FILE* _File)
{
	fwrite(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
	fwrite(&m_FOV, sizeof(float), 1, _File);
	fwrite(&m_Width, sizeof(float), 1, _File);
	fwrite(&m_Scale, sizeof(float), 1, _File);
	fwrite(&m_AspectRatio, sizeof(float), 1, _File);
	fwrite(&m_Far, sizeof(float), 1, _File);
	fwrite(&m_LayerCheck, sizeof(UINT), 1, _File);
	fwrite(&m_CameraPriority, sizeof(int), 1, _File);
}

void CCamera::LoadFromFile(FILE* _File)
{
	fread(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
	fread(&m_FOV, sizeof(float), 1, _File);
	fread(&m_Width, sizeof(float), 1, _File);
	fread(&m_Scale, sizeof(float), 1, _File);
	fread(&m_AspectRatio, sizeof(float), 1, _File);
	fread(&m_Far, sizeof(float), 1, _File);
	fread(&m_LayerCheck, sizeof(UINT), 1, _File);
	fread(&m_CameraPriority, sizeof(int), 1, _File);
}
