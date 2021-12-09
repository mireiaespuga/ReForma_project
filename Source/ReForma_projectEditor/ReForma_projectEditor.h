#pragma once

#include "UnrealEd.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"
#include "IAssetTypeActions.h"
#include "IReFormaModuleInterface.h"
#include "Developer/AssetTools/Public/IAssetTools.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Developer/AssetTools/Public/AssetToolsModule.h"
#include "../../Plugins/MySQLConnectorUE4Plugin/Source/MySQLConnectorUE4Plugin/Public/MySQLDatabase.h"
#include "Interfaces/OnlineUserInterface.h"


class FReForma_projectEditor : public IReFormaModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    virtual void AddModuleListeners() override;
    virtual void InitializeDB();
    virtual bool CloseOpenEditors();
    
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
    void setConnection(UMySQLConnection* cs) { connection = cs; };
    UMySQLConnection* getConnection() { return connection;  };
    FString  GetUserID();
    UMySQLDatabase* GetDB() { return db; };

protected:
    TSharedPtr<FExtensibilityManager> LevelEditorMenuExtensibilityManager;
    TSharedPtr<FExtender> MenuExtender;
    
    static TSharedRef<FWorkspaceItem> MenuRoot;
    TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
    FString sceneFolderName = "NONE_folderNoValid";
    UMySQLConnection* connection = NewObject<UMySQLConnection>();
    UMySQLDatabase* db = NewObject<UMySQLDatabase>();

    void MakePulldownMenu(FMenuBarBuilder& menuBuilder);
    void FillPulldownMenu(FMenuBuilder& menuBuilder);
};