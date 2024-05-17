#include "pch.h"
#include "CMissileScript.h"


CMissileScript::CMissileScript()
	: m_fSpeed(1500.f)
	, CScript(MISSILESCRIPT)
{
}

CMissileScript::~CMissileScript()
{
}

void CMissileScript::tick()
{
	return;

	Vec3 vPos = Transform()->GetRelativePos();

	vPos.y += m_fSpeed * DT;

	Transform()->SetRelativePos(vPos);	
}

void CMissileScript::SaveToFile(FILE* _File)
{
}

void CMissileScript::LoadFromFile(FILE* _File)
{
}
