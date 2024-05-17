#include "pch.h"
#include "ScriptUI.h"

#include <Scripts/CScriptMgr.h>

#include "ParamUI.h"

ScriptUI::ScriptUI()
	: ComponentUI("Script", "##Script", COMPONENT_TYPE::LIGHT2D)
	, m_TargetScript(nullptr)
{
}

ScriptUI::~ScriptUI()
{
}

void ScriptUI::SetScript(CScript* _Script)
{
	m_TargetScript = _Script;

	wstring strScriptName = CScriptMgr::GetScriptName(_Script);
	SetName(ToString(strScriptName));
	SetComopnentTitle(ToString(strScriptName));
	Activate();
}

void ScriptUI::render_update()
{
	ComponentUI::render_update();

	const vector<tScriptParam>& vecParam = m_TargetScript->GetScriptParam();

	for (size_t i = 0; i < vecParam.size(); ++i)
	{
		switch (vecParam[i].Type)
		{
		case SCRIPT_PARAM::INT:
			break;
		case SCRIPT_PARAM::FLOAT:
			ParamUI::Param_FLOAT((float*)vecParam[i].pData, vecParam[i].Desc);
			break;
		case SCRIPT_PARAM::VEC2:
			break;
		case SCRIPT_PARAM::VEC3:
			break;
		case SCRIPT_PARAM::VEC4:
			break;
		case SCRIPT_PARAM::OBJECT:
			break;		
		}		
	}
}