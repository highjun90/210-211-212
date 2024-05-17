#include "pch.h"
#include "Content.h"

#include <Engine/CAssetMgr.h>
#include <Engine/CTaskMgr.h>

#include "CImGuiMgr.h"
#include "Inspector.h"
#include "TreeUI.h"

Content::Content()
	: UI("Content", "##Content")
{
	// ContentUI 자식으로 Tree 를 지정
	m_Tree = new TreeUI("ContentTree");
	m_Tree->ShowRootNode(false);
	AddChildUI(m_Tree);
	
	// AssetMgr 의 에셋상태를 트리에 적용한다.
	ResetContent();

	// 트리에 Delegate 를 등록한다.
	m_Tree->AddSelectDelegate(this, (Delegate_1)&Content::SelectAsset);

	// 컨텐츠 폴더에 있는 에셋을 로딩한다.
	ReloadContent();
}

Content::~Content()
{
}

void Content::render_update()
{
	if (CTaskMgr::GetInst()->GetAssetEvent())
	{
		ResetContent();
	}
}




void Content::ResetContent()
{
	// Tree Clear
	m_Tree->ClearNode();

	// 루트노드 추가
	TreeNode* RootNode = m_Tree->AddTreeNode(nullptr, "Root", 0);

	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		TreeNode* CategoryNode = m_Tree->AddTreeNode(RootNode, ASSET_TYPE_STRING[i], 0);
		CategoryNode->SetFrame(true);

		const map<wstring, Ptr<CAsset>>& mapAsset = CAssetMgr::GetInst()->GetAssets((ASSET_TYPE)i);
		
		for (const auto& pair : mapAsset)
		{
			m_Tree->AddTreeNode(CategoryNode
				, string(pair.first.begin(), pair.first.end())
				, (DWORD_PTR)pair.second.Get());
		}
	}
}

void Content::SelectAsset(DWORD_PTR _Node)
{
	TreeNode* pNode = (TreeNode*)_Node;

	if (nullptr == pNode)
		return;
	
	Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
	if (nullptr == pAsset)
		return;

	// 선택한 에셋을 Inspector 에게 알려준다.
	Inspector* pInspector = (Inspector*)CImGuiMgr::GetInst()->FindUI("##Inspector");
	pInspector->SetTargetAsset(pAsset);
}


void Content::ReloadContent()
{
	// Content 폴더에 있는 모든 에셋 파일명(상대경로)을 찾아낸다.
	wstring strContentPath = CPathMgr::GetContentPath();
	FindFileName(strContentPath);

	// 찾은 파일이름으로 에셋들을 로드
	for (size_t i = 0; i < m_vecAssetFileName.size(); ++i)
	{
		ASSET_TYPE Type = GetAssetTypeByExt(m_vecAssetFileName[i]);

		switch (Type)
		{
		case ASSET_TYPE::END:
		default:
			continue;
			break;
		case ASSET_TYPE::MESH:
			CAssetMgr::GetInst()->Load<CMesh>(m_vecAssetFileName[i], m_vecAssetFileName[i]);
			break;
		case ASSET_TYPE::MESHDATA:
			//CAssetMgr::GetInst()->Load<CMeshData>(m_vecAssetFileName[i], m_vecAssetFileName[i]);
			break;
		case ASSET_TYPE::PREFAB:
			CAssetMgr::GetInst()->Load<CPrefab>(m_vecAssetFileName[i], m_vecAssetFileName[i]);
			break;
		case ASSET_TYPE::TEXTURE:
			CAssetMgr::GetInst()->Load<CTexture>(m_vecAssetFileName[i], m_vecAssetFileName[i]);
			break;
		case ASSET_TYPE::MATERIAL:
			CAssetMgr::GetInst()->Load<CMaterial>(m_vecAssetFileName[i], m_vecAssetFileName[i]);
			break;
		case ASSET_TYPE::SOUND:
			CAssetMgr::GetInst()->Load<CSound>(m_vecAssetFileName[i], m_vecAssetFileName[i]);
			break;
		}		
	}

	// 삭제된 에셋이 있으면, AssetMgr 에서도 메모리 해제 
	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		const map<wstring, Ptr<CAsset>>& mapAsset = CAssetMgr::GetInst()->GetAssets((ASSET_TYPE)i);

		for (const auto& pair : mapAsset)
		{
			// 엔진 에셋인 경우 continue 
			// 엔진 에셋은 파일로부터 로딩된 에셋이 아니라, 프로그램 실행 도중 만들어진 에셋들이기 때문
			if (pair.second->IsEngineAsset())
				continue;

			// 메모리에 로딩된 에셋의 원본파일이, content 폴더 내에서 삭제된 경우
			// 실제 로딩되어있는 에셋도 삭제시켜서 sync 를 맞춘다.
			wstring strFilePath = strContentPath + pair.second->GetRelativePath();			
			if (!exists(strFilePath))
			{
				MessageBox(nullptr, L"원본파일이 삭제되었습니다.", L"Asset 싱크", MB_OK);

				if (1 < pair.second->GetRefCount())
				{
					int value = MessageBox(nullptr, L"Asset 이 참조되고 있습니다.\n강제 삭제하시겠습니까?", L"Asset 싱크", MB_YESNO);
					if (value == IDCANCEL)
						continue;
				}

				// 에셋 매니저에서 해당 에셋을 삭제한다.
				tTask task = {};
				task.Type = TASK_TYPE::DELETE_ASSET;
				task.Param_1 = (DWORD_PTR)i;
				task.Param_2 = (DWORD_PTR)pair.second.Get();
				CTaskMgr::GetInst()->AddTask(task);
			}
		}
	}


	ResetContent();
}


void Content::FindFileName(const wstring& _Directory)
{
	// 파일 탐색 결과 저장
	WIN32_FIND_DATA FIND_DATA = {};
	
	// 탐색을 시도할 디렉터리 경로 + 찾을 파일 타입 포맷
	wstring strDirectory = _Directory + L"*.*";

	// 탐색 핸들 생성
	HANDLE hFindHandle = FindFirstFile(strDirectory.c_str(), &FIND_DATA);

	if (INVALID_HANDLE_VALUE == hFindHandle)		
		return;
		
	// 탐색 핸들을 이용해서, 파일을 다음 파일을 탐색, 더이상 없으면 false 반환
	while (FindNextFile(hFindHandle, &FIND_DATA))
	{	
		// 찾은 파일이 Directory 타입인지 확인
		if (FIND_DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// .. 이름의 폴더는 부모폴더로 가는 기능
			if (!wcscmp(L"..", FIND_DATA.cFileName))
				continue;

			// 재귀함수로 하위 폴더내에 있는 파일이름을 탐색
			FindFileName(_Directory + FIND_DATA.cFileName + L"\\");
		}
		else
		{
			// 파일 타입인 경우, 디렉터리까지 붙여서 최종 경로를 만들고, 상대경로만 추출해서 m_vecAssetFileName 에 취합
			wstring strRelativePath = CPathMgr::GetRelativePath(_Directory + FIND_DATA.cFileName);
			m_vecAssetFileName.push_back(strRelativePath);
		}
	}

	FindClose(hFindHandle);
}

ASSET_TYPE Content::GetAssetTypeByExt(const path& _relativePath)
{
	if (_relativePath.extension() == L".mesh")
		return ASSET_TYPE::MESH;
	if (_relativePath.extension() == L".mtrl")
		return ASSET_TYPE::MATERIAL;
	if (_relativePath.extension() == L".mdat")
		return ASSET_TYPE::MESHDATA;
	if (_relativePath.extension() == L".pref")
		return ASSET_TYPE::PREFAB;

	if (_relativePath.extension() == L".png" 
		|| _relativePath.extension() == L".bmp"
		|| _relativePath.extension() == L".jpg"
		|| _relativePath.extension() == L".jpeg"
		|| _relativePath.extension() == L".dds"
		|| _relativePath.extension() == L".tga")
		return ASSET_TYPE::TEXTURE;

	if (_relativePath.extension() == L".wav"
		|| _relativePath.extension() == L".mp3"
		|| _relativePath.extension() == L".ogg")
		return ASSET_TYPE::SOUND;

	if (_relativePath.extension() == L".mesh")
		return ASSET_TYPE::MESH;
	if (_relativePath.extension() == L".mesh")
		return ASSET_TYPE::MESH;

	return ASSET_TYPE::END;
}
