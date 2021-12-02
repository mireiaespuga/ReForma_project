#pragma once

#include "ReForma_projectEditor/CustomTabToolBase.h"

class DBTab : public FCustomTabToolBase
{

    virtual void OnStartupModule() override;
    virtual void OnShutdownModule() override;
    virtual void MakeMenuEntry_(FMenuBuilder& menuBuilder) ;

    protected: 
    TSharedPtr<FUICommandList> CommandList = NULL;
    // UI Command functions
    void MapCommands();
    void ReloadDBCommand();

};

