#pragma once

#include "ReForma_projectEditor/CustomTabToolBase.h"

class MatTab : public FCustomTabToolBase
{
public:
    virtual ~MatTab() {}
    virtual void OnStartupModule() override;
    virtual void OnShutdownModule() override;
    virtual void Initialize() override;
    virtual TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& TabSpawnArgs) override;
};