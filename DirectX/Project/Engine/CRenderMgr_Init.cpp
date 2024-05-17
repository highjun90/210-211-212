#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CAssetMgr.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"

#include "CStructuredBuffer.h"
#include "CMRT.h"

void CRenderMgr::init()
{
	m_Light2DBuffer = new CStructuredBuffer;
	m_Light2DBuffer->Create(sizeof(tLightInfo), 10, SB_TYPE::READ_ONLY, true);

	m_Light3DBuffer = new CStructuredBuffer;
	m_Light3DBuffer->Create(sizeof(tLightInfo), 10, SB_TYPE::READ_ONLY, true);

	m_pDebugObj = new CGameObject;
	m_pDebugObj->AddComponent(new CTransform);
	m_pDebugObj->AddComponent(new CMeshRender);

	Vec2 vRenderResolution = CDevice::GetInst()->GetRenderResolution();
	m_PostProcessTex = CAssetMgr::GetInst()->CreateTexture( L"PostProcessTex"
														 , (UINT)vRenderResolution.x
														 , (UINT)vRenderResolution.y
														 , DXGI_FORMAT_R8G8B8A8_UNORM
														 , D3D11_BIND_SHADER_RESOURCE);

	// Noise Texture Load
	m_vecNoiseTex.push_back(CAssetMgr::GetInst()->Load<CTexture>(L"texture\\noise\\noise_01.png", L"texture\\noise\\noise_01.png"));
	m_vecNoiseTex.push_back(CAssetMgr::GetInst()->Load<CTexture>(L"texture\\noise\\noise_02.png", L"texture\\noise\\noise_02.png"));
	m_vecNoiseTex.push_back(CAssetMgr::GetInst()->Load<CTexture>(L"texture\\noise\\noise_03.jpg", L"texture\\noise\\noise_03.jpg"));
		
	m_vecNoiseTex[2]->UpdateData(14);
	m_vecNoiseTex[2]->UpdateData_CS_SRV(14);
	g_global.g_NoiseTexResolution = Vec2(m_vecNoiseTex[2]->GetWidth(), m_vecNoiseTex[2]->GetHeight());

	// MRT Create
	CreateMRT();
}

void CRenderMgr::CreateMRT()
{
	Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();

	// =============
	// SwapChain MRT
	// =============
	{
		Ptr<CTexture> RTTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
		Ptr<CTexture> DSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");
		Vec4 vClearColor = (Vec4(0.3f, 0.3f, 0.3f, 1.f));

		m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->Create(&RTTex, 1, DSTex);
		m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->SetClearColor(&vClearColor, 1);
	}

	// ============
	// Deferred MRT
	// ============
	{
		Ptr<CTexture> pRTTex[4] =
		{
			CAssetMgr::GetInst()->CreateTexture(L"ColorTargetTex"
											  , vResolution.x, vResolution.y
											  , DXGI_FORMAT_R8G8B8A8_UNORM
											  , D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CAssetMgr::GetInst()->CreateTexture(L"PositionTargetTex"
											  , vResolution.x, vResolution.y
											  , DXGI_FORMAT_R32G32B32A32_FLOAT
											  , D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CAssetMgr::GetInst()->CreateTexture(L"NormalTargetTex"
											  , vResolution.x, vResolution.y
											  , DXGI_FORMAT_R32G32B32A32_FLOAT
											  , D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CAssetMgr::GetInst()->CreateTexture(L"EmissiveTargetTex"
											  , vResolution.x, vResolution.y
											  , DXGI_FORMAT_R32G32B32A32_FLOAT
											  , D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};

		Vec4 arrClearColor[4] = {
			Vec4(0.f, 0.f, 0.f, 1.f),
			Vec4(0.f, 0.f, 0.f, -1.f),
			Vec4(0.f, 0.f, 0.f, 1.f),
			Vec4(0.f, 0.f, 0.f, 1.f),
		};

		Ptr<CTexture> DSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");

		m_arrMRT[(UINT)MRT_TYPE::DEFERRED] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->Create(pRTTex, 4, DSTex);
		m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->SetClearColor(arrClearColor, 4);
	}	



	// ============
	// Light MRT
	// ============
	{
		Ptr<CTexture> pRTTex[2] =
		{
			CAssetMgr::GetInst()->CreateTexture(L"DiffuseTargetTex"
											  , vResolution.x, vResolution.y
											  , DXGI_FORMAT_R8G8B8A8_UNORM
											  , D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CAssetMgr::GetInst()->CreateTexture(L"SpecularTargetTex"
											  , vResolution.x, vResolution.y
											  , DXGI_FORMAT_R8G8B8A8_UNORM
											  , D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),			
		};

		Vec4 arrClearColor[2] = {
			Vec4(0.f, 0.f, 0.f, 1.f),
			Vec4(0.f, 0.f, 0.f, 1.f),			
		};

		m_arrMRT[(UINT)MRT_TYPE::LIGHT] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::LIGHT]->Create(pRTTex, 2, nullptr);
		m_arrMRT[(UINT)MRT_TYPE::LIGHT]->SetClearColor(arrClearColor, 2);
	}



	// =========
	// Decal MRT
	// =========
	{
		Ptr<CTexture> pRTTex[2] =
		{
			CAssetMgr::GetInst()->FindAsset<CTexture>(L"ColorTargetTex"),			
			CAssetMgr::GetInst()->FindAsset<CTexture>(L"EmissiveTargetTex"),
		};

		Vec4 arrClearColor[2] = {
			Vec4(0.f, 0.f, 0.f, 1.f),
			Vec4(0.f, 0.f, 0.f, 1.f),			
		};

		m_arrMRT[(UINT)MRT_TYPE::DECAL] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::DECAL]->Create(pRTTex, 2, nullptr);
		m_arrMRT[(UINT)MRT_TYPE::DECAL]->SetClearColor(arrClearColor, 2);
	}

	// ============
	// SHADOW_DEPTH
	// ============
	{
		Ptr<CTexture> pRTTex[1] =
		{
			CAssetMgr::GetInst()->CreateTexture(L"ShadowDepthTargetTex"
							    , 8192, 8192, DXGI_FORMAT_R32_FLOAT
							    , D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
		};

		Vec4 arrClearColor[1] = {
			Vec4(0.f, 0.f, 0.f, 0.f),			
		};

		Ptr<CTexture> pDepthTex = CAssetMgr::GetInst()->CreateTexture(L"ShadowDepthStencilTex"
								 , 8192, 8192, DXGI_FORMAT_D32_FLOAT
								 , D3D11_BIND_DEPTH_STENCIL);
								 
		m_arrMRT[(UINT)MRT_TYPE::SHADOW_DEPTH] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::SHADOW_DEPTH]->Create(pRTTex, 1, pDepthTex);
		m_arrMRT[(UINT)MRT_TYPE::SHADOW_DEPTH]->SetClearColor(arrClearColor, 1);
	}
}

void CRenderMgr::CopyRenderTargetToPostProcessTarget()
{
	Ptr<CTexture> pRTTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
	CONTEXT->CopyResource(m_PostProcessTex->GetTex2D().Get(), pRTTex->GetTex2D().Get());
}


