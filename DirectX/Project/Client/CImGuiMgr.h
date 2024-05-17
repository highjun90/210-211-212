#pragma once
#include <Engine/singleton.h>

class UI;

typedef void(UI::* Delegate_0)();
typedef void(UI::* Delegate_1)(DWORD_PTR);
typedef void(UI::* Delegate_2)(DWORD_PTR, DWORD_PTR);

typedef void (*CALL_BACK_0)(void);
typedef void (*CALL_BACK_1)(DWORD_PTR);
typedef void (*CALL_BACK_2)(DWORD_PTR, DWORD_PTR);

class CImGuiMgr :
    public CSingleton<CImGuiMgr>
{
    SINGLE(CImGuiMgr);
private:
    map<string, UI*>    m_mapUI;
    bool                m_bDemoUI;

    HANDLE              m_hNotify;


public:
    void init(HWND _hMainWnd, ComPtr<ID3D11Device> _Device, ComPtr <ID3D11DeviceContext> _Context);
    void progress();

private:
    void tick();
    void render();
    void create_ui();
    void observe_content();

public:
    UI* FindUI(const string& _strUIName);   
    void AddUI(const string& _strKey, UI* _UI);
};

