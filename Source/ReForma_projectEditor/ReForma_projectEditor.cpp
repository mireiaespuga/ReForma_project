#include "ReForma_projectEditor.h"
#include "IReFormaModuleInterface.h"
#include "MatTab/MatTab.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "Editor/ContentBrowser/Public/IContentBrowserSingleton.h"
#include "MatTab/MatComparer.h"

IMPLEMENT_GAME_MODULE(FReForma_projectEditor, ReForma_projectEditor)
TSharedRef<FWorkspaceItem> FReForma_projectEditor::MenuRoot = FWorkspaceItem::NewGroup(FText::FromString("Menu Root"));


void FReForma_projectEditor::AddModuleListeners()
{
    // add tools later
    ModuleListeners.Add(MakeShareable(new MatTab));
}

void AddAssetToDictionary(TArray< FAssetData > SelectedAssets) {
    FMatComparer MatComparer;

    for (auto selected : SelectedAssets) {
        auto* asset = selected.GetAsset();
        UMaterialInterface* assetToImport = Cast<UMaterialInterface>(asset);
        FString matname = assetToImport->GetFName().ToString();
        if (!MatComparer.AddMaterialToDict(assetToImport)) {
            const EAppReturnType::Type Choice = FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString(TEXT("Material: ") + matname + TEXT(" could not be added. \nMay already exist in dictionary")));
        }else{
            const EAppReturnType::Type Choice = FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString(TEXT("Material: ") + matname + TEXT(" was successfully added!")));
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
        
    }

    //TWeakPtr<IAssetTypeActions> Action = AssetTools.GetAssetTypeActionsForClass(UMaterialInterface::StaticClass());

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

    //menuBuilder.BeginSection("ExampleSection", FText::FromString("Section 2"));
    //menuBuilder.AddMenuSeparator(FName("Section_2"));
    //menuBuilder.EndSection();
}