#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CConstBuffer.h"

#include "CStructuredBuffer.h"

#include "CTimeMgr.h"
#include "CDevice.h"
#include "CAssetMgr.h"
#include "components.h"

#include "CLevelMgr.h"
#include "CLevel.h"

#include "CMRT.h"


CRenderMgr::CRenderMgr()
	: m_arrMRT{}
	, m_Light2DBuffer(nullptr)
	, m_pDebugObj(nullptr)
	, m_DebugPosition(true)
	, m_EditorCam(nullptr)
	, m_RenderFunc(nullptr)
{	
	m_RenderFunc = &CRenderMgr::render_play;
}

CRenderMgr::~CRenderMgr()
{
	if (nullptr != m_pDebugObj)
		delete m_pDebugObj;

	if (nullptr != m_Light2DBuffer)
		delete m_Light2DBuffer;

	if (nullptr != m_Light3DBuffer)
		delete m_Light3DBuffer;

	Delete_Array(m_arrMRT);
}

void CRenderMgr::tick()
{
	ClearMRT();
	UpdateData();

	(this->*m_RenderFunc)();
	render_debug();

	Clear();
}


void CRenderMgr::ClearMRT()
{
	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->Clear();
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->ClearRT();
	m_arrMRT[(UINT)MRT_TYPE::LIGHT]->ClearRT();
	m_arrMRT[(UINT)MRT_TYPE::SHADOW_DEPTH]->Clear();
}


void CRenderMgr::CreateDynamicShadowDepth()
{
	GetMRT(MRT_TYPE::SHADOW_DEPTH)->OMSet();

	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		// ���� �������� ��ü���� ���̸� �׸���.
		m_vecLight3D[i]->render_shadowdepth();
	}
}




void CRenderMgr::render_play()
{
	if (m_vecCam.empty())
		return;

	CreateDynamicShadowDepth();

	// ����ī�޶� �������� ������	
	if (nullptr != m_vecCam[0])
	{
		CCamera* pMainCam = m_vecCam[0];

		// �����ο� ���� ��ü �з�
		pMainCam->SortObject();

		// ����� view ��İ� proj ����� ���������� ��Ƶд�.
		g_Transform.matView = pMainCam->GetViewMat();
		g_Transform.matViewInv = pMainCam->GetViewInvMat();
		g_Transform.matProj = pMainCam->GetProjMat();
		g_Transform.matProjInv = pMainCam->GetProjInvMat();

		// Domain ������� ������
		// Deferred ��ü ������
		GetMRT(MRT_TYPE::DEFERRED)->OMSet();
		pMainCam->render_deferred();
		
		// Decal ��ü ������
		GetMRT(MRT_TYPE::DECAL)->OMSet();
		pMainCam->render_decal();

		// �׸��� ����



		// ����ó��
		// Light MRT �� ����
		GetMRT(MRT_TYPE::LIGHT)->OMSet();

		// ������ �ڽ��� ��������� �ִ� Deferred ��ü�� ���� �����.		
		for (size_t i = 0; i < m_vecLight3D.size(); ++i)
		{
			m_vecLight3D[i]->render();
		}

		// Deferred + ���� => SwapChain ���� ����
		pMainCam->Merge();

		// Foward ������
		pMainCam->render_forward();

		// ��ó�� �۾�
		pMainCam->render_postprocess();
	}

	// �߰� ����ī�޶� ���� ������
	for (int i = 1; i < m_vecCam.size(); ++i)
	{
		// Foward ������
		m_vecCam[i]->render_forward();
	}
}

void CRenderMgr::render_editor()
{
	if (nullptr == m_EditorCam)
		return;

	// ���� �������� ShadowDepthMap ����
	CreateDynamicShadowDepth();


	// �����ο� ���� ��ü �з�
	m_EditorCam->SortObject();

	// ����� view ��İ� proj ����� ���������� ��Ƶд�.
	g_Transform.matView = m_EditorCam->GetViewMat();
	g_Transform.matViewInv = m_EditorCam->GetViewInvMat();
	g_Transform.matProj = m_EditorCam->GetProjMat();
	g_Transform.matProjInv = m_EditorCam->GetProjInvMat();

	// Domain ������� ������
	// Deferred ��ü ������
	GetMRT(MRT_TYPE::DEFERRED)->OMSet();
	m_EditorCam->render_deferred();

	// Decal ��ü ������
	GetMRT(MRT_TYPE::DECAL)->OMSet();
	m_EditorCam->render_decal();

	// ����ó��
	// Light MRT �� ����
	GetMRT(MRT_TYPE::LIGHT)->OMSet();

	// ������ �ڽ��� ��������� �ִ� Deferred ��ü�� ���� �����.		
	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		m_vecLight3D[i]->render();
	}

	// Deferred + ���� => SwapChain ���� ����
	m_EditorCam->Merge();

	// Foward ������
	m_EditorCam->render_forward();

	// ��ó�� �۾�
	m_EditorCam->render_postprocess();
}

void CRenderMgr::render_debug()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	if (pCurLevel->GetState() == LEVEL_STATE::PLAY)
	{
		if (m_vecCam.empty())
			return;

		g_Transform.matView = m_vecCam[0]->GetViewMat();
		g_Transform.matProj = m_vecCam[0]->GetProjMat();
	}
	
	else
	{
		if (nullptr == m_EditorCam)
			return;

		g_Transform.matView = m_EditorCam->GetViewMat();
		g_Transform.matProj = m_EditorCam->GetProjMat();
	}


	list<tDebugShapeInfo>::iterator iter = m_DbgShapeInfo.begin();
	for (; iter != m_DbgShapeInfo.end(); )
	{
		switch ((*iter).eShape)
		{
		case DEBUG_SHAPE::RECT:
			m_pDebugObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh_Debug"));
			break;
		case DEBUG_SHAPE::CIRCLE:
			m_pDebugObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CircleMesh_Debug"));
			break;
		case DEBUG_SHAPE::CROSS:
			m_pDebugObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CrossMesh"));
			break;
		case DEBUG_SHAPE::CUBE:
			m_pDebugObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CubeMesh_Debug"));
			break;
		case DEBUG_SHAPE::SPHERE:
			m_pDebugObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
			break;
		default:
			break;
		}

		m_pDebugObj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DebugShapeMtrl"));
		m_pDebugObj->MeshRender()->GetMaterial()->SetScalarParam(SCALAR_PARAM::VEC4_0, (*iter).vColor);

		// �������� �ɼ� ����
		if ((*iter).bDepthTest)		
			m_pDebugObj->MeshRender()->GetMaterial()->GetShader()->SetDSType(DS_TYPE::NO_WRITE);		
		else
			m_pDebugObj->MeshRender()->GetMaterial()->GetShader()->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

		D3D11_PRIMITIVE_TOPOLOGY PrevTopology = m_pDebugObj->MeshRender()->GetMaterial()->GetShader()->GetTopology();
		if (DEBUG_SHAPE::CROSS == (*iter).eShape)
		{
			m_pDebugObj->MeshRender()->GetMaterial()->GetShader()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		}
		else if (DEBUG_SHAPE::SPHERE == (*iter).eShape)
		{
			m_pDebugObj->MeshRender()->GetMaterial()->GetShader()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_pDebugObj->MeshRender()->GetMaterial()->SetScalarParam(SCALAR_PARAM::INT_0, 1);
		}
		else
		{
			m_pDebugObj->MeshRender()->GetMaterial()->SetScalarParam(SCALAR_PARAM::INT_0, 0);
		}
		
		m_pDebugObj->Transform()->SetWorldMat((*iter).matWorld);
		m_pDebugObj->Transform()->UpdateData();

		m_pDebugObj->render();

		m_pDebugObj->MeshRender()->GetMaterial()->GetShader()->SetTopology(PrevTopology);

		(*iter).fLifeTime += DT;
		if ((*iter).fDuration <= (*iter).fLifeTime)
		{
			iter = m_DbgShapeInfo.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CRenderMgr::UpdateData()
{
	g_global.g_Light2DCount = (int)m_vecLight2D.size();
	g_global.g_Light3DCount = (int)m_vecLight3D.size();

	// ���� ������ ������Ʈ
	static CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL_DATA);
	pCB->SetData(&g_global);

	pCB->UpdateData();
	pCB->UpdateData_CS();

	// 2D �������� ������Ʈ
	static vector<tLightInfo> vecLight2DInfo;

	for (size_t i = 0; i < m_vecLight2D.size(); ++i)
	{
		const tLightInfo& info = m_vecLight2D[i]->GetLightInfo();
		vecLight2DInfo.push_back(info);
	}

	if (!vecLight2DInfo.empty())
	{
		m_Light2DBuffer->SetData(vecLight2DInfo.data(), (UINT)vecLight2DInfo.size());
	}	
	m_Light2DBuffer->UpdateData(11);

	vecLight2DInfo.clear();



	// 3D �������� ������Ʈ
	static vector<tLightInfo> vecLight3DInfo;

	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		const tLightInfo& info = m_vecLight3D[i]->GetLightInfo();
		vecLight3DInfo.push_back(info);
	}

	if (!vecLight3DInfo.empty())
	{
		m_Light3DBuffer->SetData(vecLight3DInfo.data(), (UINT)vecLight3DInfo.size());
	}
	m_Light3DBuffer->UpdateData(12);

	vecLight3DInfo.clear();
}

void CRenderMgr::Clear()
{
	m_vecLight2D.clear();
	m_vecLight3D.clear();
}

void CRenderMgr::RegisterCamera(CCamera* _Cam, int _Idx)
{
	if (m_vecCam.size() <= _Idx + 1)
	{
		m_vecCam.resize(_Idx + 1);
	}

	// ������ �켱������ ī�޶� �̹� �����ϸ� assert
	assert(nullptr == m_vecCam[_Idx]);

	m_vecCam[_Idx] = _Cam;
}