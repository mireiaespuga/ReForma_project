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

struct UserInfo {
    FString userId;
    FString role;
    FString permissions;
    FString password;
    
    UserInfo() :
        role(TEXT("default_user")),
        permissions(TEXT("SELECT")),
        password(TEXT("Forma1235"))
    {}
};

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
    UMySQLDatabase* GetDB() { return db; };
    void CallSaveArtistDB(const UDataTable* InDataTable, const FName InRowName);
    bool bCanUpdate = true;

    bool LoadMe();
    void SetMyRole(FString InRole) {  me->role = InRole;};
    void SetMyUserId(FString InUserId) { me->userId = InUserId; };
    void SetMyPermissions(FString InPerm) { me->permissions = InPerm; };
    void SetMyPSWRD(FString InPass) { me->password = InPass; };
    FString GetUserID() { return me->userId; };
    FString GetRole() { return me->role; };
    FString GetPSWRD() { return me->password; };
    FString GetPerm() { return me->permissions; };
    bool isArtist() { return me->role == "default_user"; };
    

protected:
    TSharedPtr<FExtensibilityManager> LevelEditorMenuExtensibilityManager;
    TSharedPtr<FExtender> MenuExtender;
    UserInfo* me = new UserInfo();
    
    static TSharedRef<FWorkspaceItem> MenuRoot;
    TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
    FString sceneFolderName = "NONE_folderNoValid";
    UMySQLConnection* connection = NewObject<UMySQLConnection>();
    UMySQLDatabase* db = NewObject<UMySQLDatabase>();

    void MakePulldownMenu(FMenuBarBuilder& menuBuilder);
    void FillPulldownMenu(FMenuBuilder& menuBuilder);
};