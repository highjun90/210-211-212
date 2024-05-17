#include "pch.h"

#include "CTaskMgr.h"
#include "CLevelMgr.h"
#include "CLevel.h"
#include "CGameObject.h"
#include "CComponent.h"

#include "CAssetMgr.h"
#include "CRenderMgr.h"

CTaskMgr::CTaskMgr()
	: m_bCreateObject(false)
	, m_bDeleteObject(false)
{

}

CTaskMgr::~CTaskMgr()
{

}

void CTaskMgr::tick()
{
	Clear();


	for (size_t i = 0; i < m_vecTask.size(); ++i)
	{
		switch (m_vecTask[i].Type)
		{
		case TASK_TYPE::CREATE_OBJECT:
		{
			int LayerIdx = (int)m_vecTask[i].Param_1;
			CGameObject* Object = (CGameObject*)m_vecTask[i].Param_2;

			CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
			pCurLevel->AddObject(Object, LayerIdx, true);

			m_bCreateObject = true;

		    if (LEVEL_STATE::PLAY == pCurLevel->GetState())
			{
				Object->begin();
			}
		}		
			break;
		case TASK_TYPE::DELETE_OBJECT:
		{
			CGameObject* pDeadObj = (CGameObject*)m_vecTask[i].Param_1;		

			list<CGameObject*> queue;
			queue.push_back(pDeadObj);

			// 레이어에 입력되는 오브젝트 포함, 그 밑에 달린 자식들까지 모두 확인
			while (!queue.empty())
			{
				CGameObject* pObject = queue.front();
				queue.pop_front();

				pObject->m_bDead = true;

				for (size_t i = 0; i < pObject->m_vecChild.size(); ++i)
				{
					queue.push_back(pObject->m_vecChild[i]);
				}								
			}

			if (m_DeleteFrameCount == 0)
				++m_DeleteFrameCount;
			else if (m_DeleteFrameCount == 2)
				m_DeleteFrameCount = 1;
		}
			break;

			
		case TASK_TYPE::ADD_ASSET:
		{
			// Param1 : Asset Adress
			CAsset* pAsset = (CAsset*)m_vecTask[i].Param_1;
			CAssetMgr::GetInst()->AddAsset(pAsset->GetName(), pAsset);
			m_bAssetChange = true;
		}
			break;	

		case TASK_TYPE::DELETE_ASSET:
		{
			// Param1 : AssetType, Param2 : Asset Adress
			ASSET_TYPE Type = (ASSET_TYPE)m_vecTask[i].Param_1;
			CAsset* pAsset = (CAsset*)m_vecTask[i].Param_2;
			CAssetMgr::GetInst()->DeleteAsset(Type, pAsset->GetKey());
			m_bAssetChange = true;
		}

			break;

		case TASK_TYPE::CHANGE_LEVELSTATE:
		{
			CLevel* pLevel = (CLevel*)m_vecTask[i].Param_1;
			LEVEL_STATE NextState = (LEVEL_STATE)m_vecTask[i].Param_2;

			pLevel->ChangeState(NextState);
		}
			break;

		case TASK_TYPE::CHANGE_LEVEL:
		{
			CLevel* pNextLevel = (CLevel*)m_vecTask[i].Param_1;
			LEVEL_STATE State = (LEVEL_STATE)m_vecTask[i].Param_2;

			CRenderMgr::GetInst()->ClearCamera();
			CLevelMgr::GetInst()->ChangeLevel_Task(pNextLevel, State);

			m_bCreateObject = true;

			break;
		}
		case TASK_TYPE::ADD_CHILD:

			break;

		case TASK_TYPE::DISCONNECT_PARENT:

			break;
		}
	}

	m_vecTask.clear();
}

void CTaskMgr::Clear()
{
	m_bCreateObject = false;
	
	// 억지
	if (1 == m_DeleteFrameCount)
	{
		++m_DeleteFrameCount;
		m_bDeleteObject = true;
	}
	else if (2 <= m_DeleteFrameCount)
	{
		m_DeleteFrameCount = 0;
		m_bDeleteObject = false;
	}		

	m_bAssetChange = false;
}
