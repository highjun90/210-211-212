#pragma once
#include "ComponentUI.h"

#include <Engine/CScript.h>

class ScriptUI :
    public ComponentUI
{
private:
    CScript* m_TargetScript;

public:
    void SetScript(CScript* _Script);

    virtual void render_update() override;


public:
    ScriptUI();
    ~ScriptUI();
};

