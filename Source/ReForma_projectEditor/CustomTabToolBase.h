#pragma once

#include "ReForma_projectEditor/ReForma_projectEditor.h"
#include "ReForma_projectEditor/IReFormaModuleInterface.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"

class FCustomTabToolBase : public IReFormaModuleListenerInterface, public TSharedFromThis< FCustomTabToolBase >
{
public:
    virtual ~FCustomTabToolBase() {}
    // IPixelopusToolBase
    virtual void OnStartupModule() override
    {
        Initialize();
        FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TabName, FOnSpawnTab::CreateRaw(this, &FCustomTabToolBase::SpawnTab))
            .SetGroup(FReForma_projectEditor::Get().GetMenuRoot())
            .SetDisplayName(TabDisplayName)
            .SetTooltipText(ToolTipText);
    };

    virtual void OnShutdownModule() override
    {
        FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabName);
    };

    // In this function set TabName/TabDisplayName/ToolTipText
    virtual void Initialize() {};
    virtual TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& TabSpawnArgs) { return SNew(SDockTab); };

    virtual void MakeMenuEntry(FMenuBuilder& menuBuilder)
    {
        FGlobalTabmanager::Get()->PopulateTabSpawnerMenu(menuBuilder, TabName);
    };

   
protected:
    FName TabName;
    FText TabDisplayName;
    FText ToolTipText;
};