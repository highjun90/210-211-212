#pragma once
#include "AssetUI.h"


class SoundUI :
    public AssetUI
{
private:
    Ptr<CSound>     m_RecentSound;

public:
    virtual void render_update() override;

public:
    SoundUI();
    ~SoundUI();
};

