#pragma once
#include "UI.h"


class TreeNode
{
private:
    TreeUI*             m_Owner;

    string              m_Name;
    string              m_ID;
    vector<TreeNode*>   m_vecChildNode;

    TreeNode*           m_ParentNode;
    DWORD_PTR           m_Data;

    bool                m_bFrame;
    bool                m_bSelected;

public:
    void SetName(string& _Name) { m_Name = _Name; }    
    void SetFrame(bool _Frame) { m_bFrame = _Frame; }

    const string& GetName() { return m_Name; }
    DWORD_PTR GetData() { return m_Data; }

private:
    void SetID(const string& _ID) { m_ID = _ID; }
    void AddChildNode(TreeNode* _Node) 
    { 
        m_vecChildNode.push_back(_Node); 
        _Node->m_ParentNode = this;
    }


public:
    void render_update();

public:
    TreeNode();
    ~TreeNode();

    friend class TreeUI;
};



class TreeUI :
    public UI
{
private:
    static  UINT    NodeID;

private:
    TreeNode*       m_Root;
    TreeNode*       m_Selected;

    TreeNode*       m_DragNode;
    TreeNode*       m_DropNode;

    bool            m_bShowRoot;
    bool            m_bDragDrop;

    UI*             m_SelectInst;
    Delegate_1      m_SelectFunc;
    bool            m_bSelectEvent;

    UI*             m_DragDropInst;
    Delegate_2      m_DragDropFunc;
    bool            m_bDragDropEvent;

public:
    virtual void render_update() override;

public:
    TreeNode* GetRootNode() { return m_Root; }
    TreeNode* GetSelectedNode() { return m_Selected; }

    void AddSelectDelegate(UI* _Inst, Delegate_1 _pFunc) { m_SelectInst = _Inst; m_SelectFunc = _pFunc; }
    void AddDragDropDelegate(UI* _Inst, Delegate_2 _pFunc) { m_DragDropInst = _Inst; m_DragDropFunc = _pFunc; }

    void ShowRootNode(bool _bShow) { m_bShowRoot = _bShow; }
    void UseDragDrop(bool _Use) { m_bDragDrop = _Use; }

    TreeNode* AddTreeNode(TreeNode* _Parent, string _strName, DWORD_PTR _dwData);
    void ClearNode()
    {
        if (nullptr != m_Root)
        {
            delete m_Root;
            m_Root = nullptr;
        }         
    }

private:
    void SetSelectedNode(TreeNode* _SelectedNode);
    void SetDragNode(TreeNode* _DragNode);
    void SetDropNode(TreeNode* _DropNode);




public:
    TreeUI(const string& _ID);
    ~TreeUI();

    friend class TreeNode;
};

