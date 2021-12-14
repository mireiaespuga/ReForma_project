#include "ReForma_projectEditor.h"
#include "IReFormaModuleInterface.h"
#include "MatTab/MatTab.h"
#include "DBTab/DBTab.h"
#include "Misc/Paths.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "Editor/ContentBrowser/Public/IContentBrowserSingleton.h"
#include "MatTab/MatComparer.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "DataTableEditor/Public/DataTableEditorModule.h"

IMPLEMENT_GAME_MODULE(FReForma_projectEditor, ReForma_projectEditor)
TSharedRef<FWorkspaceItem> FReForma_projectEditor::MenuRoot = FWorkspaceItem::NewGroup(FText::FromString("Menu Root"));
FMatComparer MatComparer_;

void FReForma_projectEditor::AddModuleListeners()
{
    // add tools later
    ModuleListeners.Add(MakeShareable(new MatTab));
    ModuleListeners.Add(MakeShareable(new DBTab));
}

bool FReForma_projectEditor::CloseOpenEditors() {
    EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::OkCancel, EAppReturnType::Cancel, FText::FromString(TEXT("IMPORTANT! All editor tabs will be closed before reloading database. Save your changes beforehand. \n\n Do you want to continue?")));
    
    if (ReturnType == EAppReturnType::Ok)
    {
        //FAssetEditorManager::Get().CloseAllAssetEditors();
        GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseAllAssetEditors();
        return true;
    }
    else return false;
    
}

void FReForma_projectEditor::CallUpdateDB(const UDataTable* InDataTable, const FName InRowName) {
    if(bCanUpdate) DBTab::UpdateDB(InDataTable, InRowName);
}

bool FReForma_projectEditor::LoadMe() {

    //CONFIG FILEEEEE!!!! DEALS W/ USER TABLE PLUS PERMISSIONS!
    FString directory = "C:/Users/mysql_unreal_userinfo.txt", loadedFile;
    TArray<FString> dataLines;
    if (FFileHelper::LoadFileToString(loadedFile, *directory)) {
        int32 lineCount = loadedFile.ParseIntoArray(dataLines, _T(";"), true);
        for (auto dataline : dataLines) {
            FString value, param;
            dataline = dataline.Replace(TEXT(";"), TEXT(""));
            dataline.Split(TEXT("="), &param, &value);
            if (param == "username") SetMyUserId(value);
            if (param == "role") SetMyRole(value);
            if (param == "password") SetMyPSWRD(value);
        }
        return true;
    }
    else {
        FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString(TEXT("Something went wrong..\n\n Check you have your mysql_unreal_userinfo.txt file in C:/Users/ directory ")));
        return false;
    }

    //UE_LOG(LogTemp, Warning, TEXT("%s"), *loadedFile); //C:/Users/Mireia Espuga/Desktop/ReForma_project/Content/
    //FString username, right;
    //bool result;
    //FString dir = FPaths::ProjectContentDir();
    //result = dir.Split(TEXT("Users/"), NULL, &right);
    //if (result) {
    //    result = right.Split(TEXT("/"), &username, NULL);
    //    username = username.Replace(TEXT(" "), TEXT(""));
    //}

    //APlayerController* const PlayerController = GEngine->GetWorld()->GetFirstPlayerController();
    
    //IOnlineUser* OnlineSub = IOnlineUser::GetAllUserInfo(FGenericPlatformMisc::GetEpicAccountId())
    //if (OnlineSub)
    //{
    //    // Get the Session Interface to clear the Delegate
    //    IOnlineUserPtr userint = OnlineSub->GetUserInterface();
    //    if (userint.IsValid())
    //    {
    //        // Clear the delegate, since we are done with this call

    //    }
    //}
  /*  if (!result) {
        FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString(TEXT("Something went wrong with selecting user...")));
        return "NONE";
    }else return username.ToLower();
    */
}

void AddAssetToDictionary(TArray< FAssetData > SelectedAssets) {
    
    FString matname, errormats = "";
    int addedMats = 0;

    for (auto selected : SelectedAssets) {
        auto* asset = selected.GetAsset();
        UMaterialInterface* assetToImport = Cast<UMaterialInterface>(asset);
        matname = assetToImport->GetFName().ToString();
        if (!MatComparer_.AddMaterialToDict(assetToImport)) {
            errormats.Append(matname + " , ");
        }else{
            addedMats++;
        }
    }

    //feedback
    if (addedMats > 0) {
        if(addedMats == 1) FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString(TEXT("Material: ") + matname + TEXT(" was successfully added!")));
        else  FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString(FString::Printf(TEXT("%d materials were added successfully!"), addedMats)));
    }
    else {
        FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString(TEXT("Material(s): ") + errormats + TEXT(" could not be added. \nMay already exist in dictionary")));
    }

}

void FReForma_projectEditor::InitializeDB(){ 
    
    if (IsAvailable()) {
        bCanUpdate = false;
        if (FReForma_projectEditor::Get().LoadMe()) {
            UMySQLConnection* cs = FReForma_projectEditor::Get().GetDB()->MySQLInitConnection("localhost", GetRole(), GetPSWRD(), "ReFormaDB");//"localhost", "root", "ReForma2021#!", "ReFormaDB");
            FReForma_projectEditor::Get().setConnection(cs);
            MatComparer_.initDB(); //init db connection
            bCanUpdate = true;
        }
    }
}  

void PopulateFormaActionsMenu(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets) {
    MenuBuilder.AddMenuEntry(
        FText::FromString("Include in Dictionary"),
        FText::FromString("Add material entry to dictionary "),
        FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetActions.ReimportAsset"),
        FUIAction(FExecuteAction::CreateStatic(&AddAssetToDictionary, SelectedAssets), FCanExecuteAction()));
   
}


TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets) {
        TSharedRef<FExtender> Extender = MakeShared<FExtender>();
        
        // Run through the assets to determine if any meet our criteria
        bool bShouldExtendAssetActions = false;
        for (const FAssetData& Asset : SelectedAssets)
        {
            if (Asset.AssetClass == UMaterial::StaticClass()->GetFName() || Asset.AssetClass == UMaterialInstance::StaticClass()->GetFName() ||
                Asset.AssetClass == UMaterialInstanceConstant::StaticClass()->GetFName())
            {
                UMaterialInterface* MaterialInterface = Cast< UMaterialInterface >(Asset.GetAsset()); // Need to load the asset at this point to figure out the type of the AssetImportData

                if (MaterialInterface && MaterialInterface->AssetImportData)/* && MaterialInterface->AssetImportData->IsA<UDatasmithAssetImportData>()) ||
                    FDatasmithObjectTemplateUtils::HasObjectTemplates(MaterialInterface)*/
                {
                    bShouldExtendAssetActions = true;
                    break;
                }
            }      
        }

        if (bShouldExtendAssetActions) {
            // Add the FormaDisseny actions sub-menu extender
            Extender->AddMenuExtension("GetAssetActions", EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateLambda(
                [SelectedAssets](FMenuBuilder& MenuBuilder)
                {
                    MenuBuilder.AddSubMenu(
                        FText::FromString("FormaDisseny"),
                        FText::FromString("FormaDisseny related tools"),
                        FNewMenuDelegate::CreateStatic(&PopulateFormaActionsMenu, SelectedAssets),
                        false,
                        FSlateIcon());
                }));
        }
    

        return Extender;
}


void FReForma_projectEditor::StartupModule()
{
    if (!IsRunningCommandlet())
    {
        FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
        LevelEditorMenuExtensibilityManager = LevelEditorModule.GetMenuExtensibilityManager();
        MenuExtender = MakeShareable(new FExtender);
        MenuExtender->AddMenuBarExtension("Window", EExtensionHook::After, NULL, FMenuBarExtensionDelegate::CreateRaw(this, &FReForma_projectEditor::MakePulldownMenu));
        LevelEditorMenuExtensibilityManager->AddExtender(MenuExtender);
    
        FDelegateHandle ContentBrowserExtenderDelegateHandle;
        FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
        TArray< FContentBrowserMenuExtender_SelectedAssets >& CBMenuExtenderDelegates = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();

        CBMenuExtenderDelegates.Add(FContentBrowserMenuExtender_SelectedAssets::CreateStatic(&OnExtendContentBrowserAssetSelectionMenu));
        ContentBrowserExtenderDelegateHandle = CBMenuExtenderDelegates.Last().GetHandle();
        

        if(IsAvailable()) FReForma_projectEditor::Get().InitializeDB();
    }
    IReFormaModuleInterface::StartupModule();
}


void FReForma_projectEditor::ShutdownModule()
{
    // Unregister all the asset types that we registered
    if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
    {
        IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
        for (int32 i = 0; i < CreatedAssetTypeActions.Num(); ++i)
        {
            AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[i].ToSharedRef());
        }
    }
    CreatedAssetTypeActions.Empty();

    IReFormaModuleInterface::ShutdownModule();
}

void FReForma_projectEditor::AddMenuExtension(const FMenuExtensionDelegate& extensionDelegate, FName extensionHook, const TSharedPtr<FUICommandList>& CommandList, EExtensionHook::Position position)
{
    MenuExtender->AddMenuExtension(extensionHook, position, CommandList, extensionDelegate);
}

void FReForma_projectEditor::MakePulldownMenu(FMenuBarBuilder& menuBuilder)
{
    menuBuilder.AddPullDownMenu(
        FText::FromString("FormaDisseny"),
        FText::FromString("FormaDisseny related tools"),
        FNewMenuDelegate::CreateRaw(this, &FReForma_projectEditor::FillPulldownMenu),
        "FormaDisseny",
        FName(TEXT("FormaDissenyMenu"))
    );
}

void FReForma_projectEditor::FillPulldownMenu(FMenuBuilder& menuBuilder)
{
    // just a frame for tools to fill in
    menuBuilder.BeginSection("FormaSection", FText::FromString("Materials"));
    menuBuilder.AddMenuSeparator(FName("Materials"));
    menuBuilder.EndSection();

    menuBuilder.BeginSection("FormaSection", FText::FromString("Database"));
    menuBuilder.AddMenuSeparator(FName("DataBase"));
    menuBuilder.EndSection();

}

