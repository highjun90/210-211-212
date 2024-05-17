#pragma once
#include "ComponentUI.h"
class MeshRenderUI :
    public ComponentUI
{

public:
    virtual void render_update() override;



public:
    void MeshSelect(DWORD_PTR _ptr);
    void MaterialSelect(DWORD_PTR _ptr);

public:
    MeshRenderUI();
    ~MeshRenderUI();
};

