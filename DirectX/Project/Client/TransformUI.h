#pragma once
#include "ComponentUI.h"

#include <Engine/CGameObject.h>

class TransformUI :
    public ComponentUI
{
public:
    virtual void render_update() override;
public:
    TransformUI();
    ~TransformUI();
};

