#pragma once

enum class TASK_TYPE
{
	// Param1 : Layer Index, Param2 : Object Adress
	CREATE_OBJECT,

	// Param1 : Object Adress
	DELETE_OBJECT,

	// Param1 : Asset Adress 
	ADD_ASSET,
	
	// Param1 : AssetType, Param2 : Asset Adress
	DELETE_ASSET, 

	// Param1 : Level, Param2 : LEVEL_STATE
	CHANGE_LEVELSTATE,
		
	// Param1 : Level Adress, Param2 : Next Level Start State
	CHANGE_LEVEL,

	// Param1 : Parent Object, Param2 : Child Object
	ADD_CHILD,

	// Param1 : Parent Object, Param2 : Child Object
	DISCONNECT_PARENT,
};

struct tTask
{
	TASK_TYPE Type;
	UINT_PTR  Param_1;
	UINT_PTR  Param_2;
};


class CTaskMgr
	: public CSingleton<CTaskMgr>
{
	SINGLE(CTaskMgr);

private:
	vector<tTask>	m_vecTask;

	bool			m_bCreateObject;
	bool			m_bDeleteObject;
	bool			m_bAssetChange;
	int				m_DeleteFrameCount;

public:
	void tick();
	void AddTask(const tTask& _Task) { m_vecTask.push_back(_Task); }
	bool GetObjectEvent() { return m_bCreateObject || m_bDeleteObject; }
	bool GetAssetEvent() { return m_bAssetChange; }

private:
	void Clear();
};

