#include "pch.h"
#include "CAsset.h"

CAsset::CAsset(ASSET_TYPE _Type, bool _bEngineAsset)
	: m_Type(_Type)
	, m_bEngineAsset(_bEngineAsset)
	, m_RefCount(0)
{
}

CAsset::CAsset(const CAsset& _Origin)
	: CEntity(_Origin)
	, m_Type(_Origin.m_Type)
	, m_bEngineAsset(_Origin.m_bEngineAsset)
	, m_RefCount(0)
{
}

CAsset::~CAsset()
{
}
