#pragma once

#include "UnrealEd.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"
#include "IAssetTypeActions.h"
#include "IReFormaModuleInterface.h"

class FReForma_projectEditor : public IReFormaModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    virtual void AddModuleListeners() override;

    static inline FReForma_projectEditor& Get()
    {
        return FModuleManager::LoadModuleChecked< FReForma_projectEditor >("ReForma_projectEditor");
    }

    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("ReForma_projectEditor");
    }

    void AddMenuExtension(const FMenuExtensionDelegate& extensionDelegate, FName extensionHook, const TSharedPtr<FUICommandList>& CommandList = NULL, EExtensionHook::Position position = EExtensionHook::Before);
    TSharedRef<FWorkspaceItem> GetMenuRoot() { return MenuRoot; };


protected:
    TSharedPtr<FExtensibilityManager> LevelEditorMenuExtensibilityManager;
    TSharedPtr<FExtender> MenuExtender;

    static TSharedRef<FWorkspaceItem> MenuRoot;

    void MakePulldownMenu(FMenuBarBuilder& menuBuilder);
    void FillPulldownMenu(FMenuBuilder& menuBuilder);
};