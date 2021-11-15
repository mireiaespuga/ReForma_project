#pragma once

#include "Modules/ModuleManager.h"

class IReFormaModuleListenerInterface
{
public:
    virtual void OnStartupModule() {};
    virtual void OnShutdownModule() {};
};

class IReFormaModuleInterface : public IModuleInterface
{
public:
    void StartupModule() override
    {
        if (!IsRunningCommandlet())
        {
            AddModuleListeners();
            for (int32 i = 0; i < ModuleListeners.Num(); ++i)
            {
                ModuleListeners[i]->OnStartupModule();
            }
        }
    }

    void ShutdownModule() override
    {
        for (int32 i = 0; i < ModuleListeners.Num(); ++i)
        {
            ModuleListeners[i]->OnShutdownModule();
        }
    }

    virtual void AddModuleListeners() {};

protected:
    TArray<TSharedRef<IReFormaModuleListenerInterface>> ModuleListeners;
};