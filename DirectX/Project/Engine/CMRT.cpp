#include "pch.h"
#include "CMRT.h"

#include "CDevice.h"


CMRT::CMRT()
	: m_RTView{}
	, m_ClearColor{}
	, m_RTCount(0)
	, m_ViewPort{}
{
}

CMRT::~CMRT()
{
}

void CMRT::Create(Ptr<CTexture>* _pArrTex, UINT _RTCount, Ptr<CTexture> _DSTex)
{
	assert(_RTCount <= 8);

	for (m_RTCount = 0; m_RTCount < _RTCount; ++m_RTCount)
	{
		m_arrRTTex[m_RTCount] = _pArrTex[m_RTCount];
		m_RTView[m_RTCount] = m_arrRTTex[m_RTCount]->GetRTV().Get();
	}

	m_DSTex = _DSTex;

	// ViewPort 설정
	m_ViewPort.MinDepth = 0.f;
	m_ViewPort.MaxDepth = 1.f;
	m_ViewPort.Width = m_arrRTTex[0]->GetWidth();
	m_ViewPort.Height= m_arrRTTex[0]->GetHeight();
}

void CMRT::SetClearColor(Vec4* _arrClearColor, UINT _RTCount)
{
	for (UINT i = 0; i < _RTCount; ++i)
	{
		m_ClearColor[i] = _arrClearColor[i];
	}	
}

void CMRT::OMSet()
{
	if (nullptr == m_DSTex)
	{
		CONTEXT->OMSetRenderTargets(m_RTCount, m_RTView, nullptr);
	}
	else
	{
		CONTEXT->OMSetRenderTargets(m_RTCount, m_RTView, m_DSTex->GetDSV().Get());
	}	

	// ViewPort 설정
	CONTEXT->RSSetViewports(1, &m_ViewPort);
}

void CMRT::Clear()
{
	ClearRT();

	ClearDS();
}

void CMRT::ClearRT()
{
	for (UINT i = 0; i < m_RTCount; ++i)
	{
		CONTEXT->ClearRenderTargetView(m_RTView[i], m_ClearColor[i]);
	}
}

void CMRT::ClearDS()
{
	CONTEXT->ClearDepthStencilView(m_DSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}
