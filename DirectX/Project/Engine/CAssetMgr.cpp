#include "pch.h"
#include "CAssetMgr.h"

#include "CMesh.h"
#include "CGraphicsShader.h"
#include "CMaterial.h"

#include "CSound.h"

CAssetMgr::CAssetMgr()
{
	
}

CAssetMgr::~CAssetMgr()
{
	
}

void CAssetMgr::AddAsset(const wstring& _strKey, CAsset* _Asset)
{
	ASSET_TYPE Type = _Asset->GetType();

	map<wstring, Ptr<CAsset>>::iterator iter = m_mapAsset[(UINT)Type].find(_strKey);
	assert(iter == m_mapAsset[(UINT)Type].end());

	_Asset->SetKey(_strKey);
	_Asset->SetRelativePath(_strKey);
	m_mapAsset[(UINT)Type].insert(make_pair(_strKey, _Asset));
}

Ptr<CTexture> CAssetMgr::CreateTexture(const wstring& _strKey
									 , UINT _Width, UINT _Height, DXGI_FORMAT _Format
									 , UINT _Flag, D3D11_USAGE _Usage)
{
	// 생성하려는 텍스쳐와 동일한 키의 텍스쳐가 이미 AssetMgr 에 있다면
	Ptr<CTexture> pTex = FindAsset<CTexture>(_strKey);	
	assert(!pTex.Get());

	pTex = new CTexture(true);

	if (FAILED(pTex->Create(_Width, _Height, _Format, _Flag, _Usage)))
	{
		MessageBox(nullptr, L"텍스쳐 생성 실패", L"리소스 생성 실패", MB_OK);
		return nullptr;
	}

	AddAsset<CTexture>(_strKey, pTex.Get());

	return pTex;
}

Ptr<CTexture> CAssetMgr::CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _tex2D)
{
	// 생성하려는 텍스쳐와 동일한 키의 텍스쳐가 이미 AssetMgr 에 있다면
	Ptr<CTexture> pTex = FindAsset<CTexture>(_strKey);
	assert(!pTex.Get());

	pTex = new CTexture(true);

	if (FAILED(pTex->Create(_tex2D)))
	{
		MessageBox(nullptr, L"텍스쳐 생성 실패", L"리소스 생성 실패", MB_OK);
		return nullptr;
	}

	AddAsset<CTexture>(_strKey, pTex.Get());

	return pTex;
}

void CAssetMgr::DeleteAsset(ASSET_TYPE _Type, const wstring& _strKey)
{
	map<wstring, Ptr<CAsset>>::iterator iter = m_mapAsset[(UINT)_Type].find(_strKey);

	assert(!(iter == m_mapAsset[(UINT)_Type].end()));

	m_mapAsset[(UINT)_Type].erase(iter);
}

void CAssetMgr::GetAssetName(ASSET_TYPE _Type, vector<string>& _Out)
{
	for (const auto& pair : m_mapAsset[(UINT)_Type])
	{
		_Out.push_back(ToString(pair.first));
	}
}
