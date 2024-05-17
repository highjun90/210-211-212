#include "pch.h"
#include "TreeUI.h"

#include <Engine/CKeyMgr.h>

TreeNode::TreeNode()
	: m_bFrame(false)
{
}

TreeNode::~TreeNode()
{
	Delete_Vec(m_vecChildNode);
}


void TreeNode::render_update()
{
	string strID = m_Name + m_ID;

	UINT Flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (m_bFrame)
		Flag |= ImGuiTreeNodeFlags_Framed;
	if (m_vecChildNode.empty())	
		Flag |= ImGuiTreeNodeFlags_Leaf;		
	if (m_bSelected)
		Flag |= ImGuiTreeNodeFlags_Selected;

	if (m_bFrame && m_vecChildNode.empty())
		strID = "   " + strID;

	if (ImGui::TreeNodeEx(strID.c_str(), Flag))
	{
		if (ImGui::BeginDragDropSource())
		{			
			ImGui::SetDragDropPayload(m_Owner->GetID().c_str(), &m_Data, sizeof(DWORD_PTR));
			ImGui::Text(m_Name.c_str());
			ImGui::EndDragDropSource();

			// Tree 에 자신이 Drag 된 노드임을 알린다.
			m_Owner->SetDragNode(this);
		}

		else if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(m_Owner->GetID().c_str());
			if (payload)
			{
				m_Owner->SetDropNode(this);
			}
			ImGui::EndDragDropTarget();
		}

		else
		{
			if ( KEY_RELEASED(KEY::LBTN) && ImGui::IsItemHovered(ImGuiHoveredFlags_None))
			{
				m_Owner->SetSelectedNode(this);
			}
		}

		for (size_t i = 0; i < m_vecChildNode.size(); ++i)
		{
			m_vecChildNode[i]->render_update();
		}

		ImGui::TreePop();
	}
	else
	{
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload(m_Owner->GetID().c_str(), &m_Data, sizeof(DWORD_PTR));
			ImGui::Text(m_Name.c_str());
			ImGui::EndDragDropSource();

			// Tree 에 자신이 Drag 된 노드임을 알린다.
			m_Owner->SetDragNode(this);
		}
		else
		{
			if (KEY_RELEASED(KEY::LBTN) && ImGui::IsItemHovered(ImGuiHoveredFlags_None))
			{
				m_Owner->SetSelectedNode(this);
			}
		}
	}
}





UINT TreeUI::NodeID = 0;



TreeUI::TreeUI(const string& _ID)
	: UI("", _ID)
	, m_bShowRoot(true)
	, m_bDragDrop(false)
{	
}

TreeUI::~TreeUI()
{
	delete m_Root;
}

void TreeUI::render_update()
{
	if (nullptr == m_Root)
		return;
	
	if (m_bShowRoot)
	{
		m_Root->render_update();
	}
	else
	{
		for (size_t i = 0; i < m_Root->m_vecChildNode.size(); ++i)
		{
			m_Root->m_vecChildNode[i]->render_update();
		}
	}

	







	// Delegate 호출
	if (m_bSelectEvent)
	{
		if (m_SelectInst && m_SelectFunc)
		{
			(m_SelectInst->*m_SelectFunc)((DWORD_PTR)m_Selected);
		}
	}


	// 드래그 대상을 특정 노드가 아닌 공중드랍 시킨 경우
	if (KEY_RELEASED(KEY::LBTN) && m_DragNode && !m_DropNode)
	{
		if (m_DragDropInst && m_DragDropFunc)
		{
			(m_DragDropInst->*m_DragDropFunc)((DWORD_PTR)m_DropNode, (DWORD_PTR)m_DragNode);
		}		
		m_DragNode = nullptr;
	}
	else if (m_bDragDropEvent)
	{
		if (m_DragDropInst && m_DragDropFunc)
		{
			(m_DragDropInst->*m_DragDropFunc)((DWORD_PTR)m_DropNode, (DWORD_PTR)m_DragNode);
		}

		m_DropNode = nullptr;
		m_DragNode = nullptr;
	}

	m_bSelectEvent = false;
	m_bDragDropEvent = false;
}

TreeNode* TreeUI::AddTreeNode(TreeNode* _Parent, string _strName, DWORD_PTR _dwData)
{
	TreeNode* pNewNode = new TreeNode;
	pNewNode->m_Data = _dwData;
	pNewNode->SetName(_strName);

	// 노드마다 겹치지않는 숫자를 ## 뒤에 ID 로 붙인다.
	UINT id = NodeID++;

	char buff[50] = {};
	sprintf_s(buff, "##%d", id);
	pNewNode->SetID(buff);

	if (nullptr == _Parent)
	{
		assert(!m_Root);

		m_Root = pNewNode;
	}
	else
	{
		_Parent->AddChildNode(pNewNode);
	}

	pNewNode->m_Owner = this;

	return pNewNode;
}

void TreeUI::SetSelectedNode(TreeNode* _SelectedNode)
{
	if (m_Selected)
	{
		m_Selected->m_bSelected  = false;
	}

	m_Selected = _SelectedNode;

	if (nullptr != m_Selected)
	{
		m_Selected->m_bSelected = true;
	}		

	m_bSelectEvent = true;
}

void TreeUI::SetDragNode(TreeNode* _DragNode)
{
	m_DragNode = _DragNode;
}

void TreeUI::SetDropNode(TreeNode* _DropNode)
{
	m_DropNode = _DropNode;
	m_bDragDropEvent = true;
}
