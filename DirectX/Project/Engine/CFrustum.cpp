#include "pch.h"
#include "CFrustum.h"

#include "CCamera.h"

CFrustum::CFrustum()
	: m_Owner(nullptr)
	, m_ProjPos{}
	, m_arrFace{}
{
	// ������ǥ�� �ʱ� ��ǥ ����
	//     4 -- 5
	//   / |   /|
	//  /  7 -/ 6
	// 0 -- 1  /
	// | /  | /
	// 3 -- 2
	m_ProjPos[0] = Vec3(-1.f, 1.f, 0.f);
	m_ProjPos[1] = Vec3(1.f, 1.f, 0.f);
	m_ProjPos[2] = Vec3(1.f, -1.f, 0.f);
	m_ProjPos[3] = Vec3(-1.f, -1.f, 0.f);

	m_ProjPos[4] = Vec3(-1.f, 1.f, 1.f);
	m_ProjPos[5] = Vec3(1.f, 1.f, 1.f);
	m_ProjPos[6] = Vec3(1.f, -1.f, 1.f);
	m_ProjPos[7] = Vec3(-1.f, -1.f, 1.f);
}

CFrustum::~CFrustum()
{
}


void CFrustum::finaltick()
{
	Matrix matInv = m_Owner->GetProjInvMat() * m_Owner->GetViewInvMat();

	Vec3 vWorld[8] = {};
	for (int i = 0; i < 8; ++i)
	{
		vWorld[i] = XMVector3TransformCoord(m_ProjPos[i], matInv);
	}

	if (m_Owner->GetOwner()->GetName() == L"EditorCam")
	{
		int a = 0;
	}

	// WorldSpace �󿡼� ī�޶��� �þ߸� ǥ���ϴ� 6 ���� ��� ����

}

bool CFrustum::FrustumCheck(Vec3 _vWorldPos)
{
	return false;
}