#pragma once

#include "UnrealEd.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"
#include "IAssetTypeActions.h"
#include "IReFormaModuleInterface.h"
#include "Developer/AssetTools/Public/IAssetTools.h"
#include "Developer/AssetTools/Public/AssetToolsModule.h"


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
    FString GetFolderName() { return sceneFolderName; };
    void setFolderName(FString name) { sceneFolderName = name; };

protected:
    TSharedPtr<FExtensibilityManager> LevelEditorMenuExtensibilityManager;
    TSharedPtr<FExtender> MenuExtender;
    TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
    static TSharedRef<FWorkspaceItem> MenuRoot;
    FString sceneFolderName = "NONE_folderNoValid";

    void MakePulldownMenu(FMenuBarBuilder& menuBuilder);
    void FillPulldownMenu(FMenuBuilder& menuBuilder);
};