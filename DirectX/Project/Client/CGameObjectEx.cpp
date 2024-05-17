#include "pch.h"
#include "CGameObjectEx.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>

#include <Engine/components.h>

void CGameObjectEx::finaltick()
{
	for (UINT i = 0; i < UINT(COMPONENT_TYPE::END); ++i)
	{
		if (nullptr != GetComponent((COMPONENT_TYPE)i))
		{
			GetComponent((COMPONENT_TYPE)i)->finaltick();
		}
	}

	vector<CGameObject*>::const_iterator iter = GetChild().begin();
	for (; iter != GetChild().end(); 	++iter)
	{
		(*iter)->finaltick();
	}
}