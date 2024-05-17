#pragma once
#include "CAsset.h"

#include "CTexture.h"
#include "CGraphicsShader.h"

class CMaterial :
    public CAsset
{
private:
    tMtrlConst				m_Const;
	Ptr<CTexture>			m_arrTex[(UINT)TEX_PARAM::END];
    Ptr<CGraphicsShader>    m_pShader;
    
public:
    void SetShader(Ptr<CGraphicsShader> _Shader) { m_pShader = _Shader; }
	Ptr<CGraphicsShader> GetShader() { return m_pShader; }

    template<typename T>
    void SetScalarParam(SCALAR_PARAM _ParamType, const T& _Value);
	void SetTexParam(TEX_PARAM _Param, Ptr<CTexture> _pTex);

	void* GetScalarParam(SCALAR_PARAM _ParamType);
	Ptr<CTexture> GetTexParam(TEX_PARAM _ParamType) { return m_arrTex[(UINT)_ParamType]; }

    void UpdateData();

	void operator = (const CMaterial& _OtherMtrl)
	{
		SetName(_OtherMtrl.GetName());

		m_Const = _OtherMtrl.m_Const;

		for (UINT i = 0; i < (UINT)TEX_PARAM::END; ++i)
		{
			m_arrTex[i] = _OtherMtrl.m_arrTex[i];
		}

		m_pShader = _OtherMtrl.m_pShader;
	}

	virtual int Save(const wstring& _strRelativePath);
	virtual int Load(const wstring& _strFilePath);

	CLONE(CMaterial);
public:
	CMaterial(bool _bEngine = false);
    ~CMaterial();
};


template<typename T>
void CMaterial::SetScalarParam(SCALAR_PARAM _ParamType, const T& _Value)
{
	const T* pValue = &_Value;

	switch (_ParamType)
	{
	case SCALAR_PARAM::INT_0:
	case SCALAR_PARAM::INT_1:
	case SCALAR_PARAM::INT_2:
	case SCALAR_PARAM::INT_3:
	/*	if constexpr (std::is_same_v<T, int>) {

		}*/
		m_Const.iArr[(UINT)_ParamType] = *((int*)pValue);
		break;

	case SCALAR_PARAM::FLOAT_0:
	case SCALAR_PARAM::FLOAT_1:
	case SCALAR_PARAM::FLOAT_2:
	case SCALAR_PARAM::FLOAT_3:
		m_Const.fArr[(UINT)_ParamType - (UINT)SCALAR_PARAM::FLOAT_0] = *((float*)pValue);
		break;

	case SCALAR_PARAM::VEC2_0:
	case SCALAR_PARAM::VEC2_1:
	case SCALAR_PARAM::VEC2_2:
	case SCALAR_PARAM::VEC2_3:
		m_Const.v2Arr[(UINT)_ParamType - (UINT)SCALAR_PARAM::VEC2_0] = *((Vec2*)pValue);


		break;
	case SCALAR_PARAM::VEC4_0:
	case SCALAR_PARAM::VEC4_1:
	case SCALAR_PARAM::VEC4_2:
	case SCALAR_PARAM::VEC4_3:
		m_Const.v4Arr[(UINT)_ParamType - (UINT)SCALAR_PARAM::VEC4_0] = *((Vec4*)pValue);

		break;
	case SCALAR_PARAM::MAT_0:
	case SCALAR_PARAM::MAT_1:
	case SCALAR_PARAM::MAT_2:
	case SCALAR_PARAM::MAT_3:
		m_Const.matArr[(UINT)_ParamType - (UINT)SCALAR_PARAM::MAT_0] = *((Matrix*)pValue);
		break;
	}
}