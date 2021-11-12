#include "MatTabPanel.h"
#include "ReForma_projectEditor/ReForma_projectEditor.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "PropertyCustomizationHelpers.h"
#include "Engine/UserDefinedStruct.h"
#include "Dom/JsonObject.h"
#include "Misc/Paths.h"
#include "Widgets/Views/SListView.h"

#define LOCTEXT_NAMESPACE "SMatTabPanel"

void SMatTabPanel::Construct(const FArguments& InArgs)
{
    tool = InArgs._Tool;
    if (tool.IsValid())
    {
        // do anything you need from tool object
    }
    TSharedRef<SWidget> AssetPickerWidget = SNew(SObjectPropertyEntryBox)
        .ObjectPath(TEXT("/Game/Datasmith/test_scene/mymats"))//(this, &SMaterialAnalyzer::GetCurrentAssetPath)
        .AllowedClass(UMaterialInterface::StaticClass())
        //.OnObjectChanged()//(this, &SMaterialAnalyzer::OnAssetSelected)
        .AllowClear(false)
        .DisplayUseSelected(true)
        .DisplayBrowse(true)
        .NewAssetFactories(TArray<UFactory*>())
        .IsEnabled(true);//(this, &SMaterialAnalyzer::IsMaterialSelectionAllowed);

    ChildSlot
        [
            SNew(SScrollBox)
            + SScrollBox::Slot()
            .VAlign(VAlign_Top)
            .Padding(5)
            [
                SNew(SBorder)
                .BorderBackgroundColor(FColor(192, 192, 192, 255))
                .Padding(15.0f)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("Material Wizard")))
                ]
                [
                     SNew(SVerticalBox) 
                     + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0.f, 5.f, 0.f, 2.f)
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(2, 0, 0, 0)
                        .VAlign(VAlign_Center)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("Browse for imported datasmith scene folder:   ")))
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(2, 0, 0, 0)
                        .VAlign(VAlign_Center)
                        [
                             SNew(SDirectoryPicker)
                            .Message(FText::FromString("Choose Datasmith Scene folder"))
                            .Directory(SMatTabPanel::GetInitialPath())
                            .OnDirectoryChanged(this, &SMatTabPanel::SetCurrentFolderPath)
                        ]
                    ]
                     + SVerticalBox::Slot()
                     .AutoHeight()
                     .Padding(0.f, 5.f, 0.f, 0.f)
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(2, 0, 0, 0)
                        .VAlign(VAlign_Center)
                        [

                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("Select CSV destination folder:   ")))
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(2, 0, 0, 0)
                        .VAlign(VAlign_Center)
                            [
                                SNew(SDirectoryPicker)
                                .Message(FText::FromString("Choose CSV destination folder"))
                                .OnDirectoryChanged(this, &SMatTabPanel::SetCSVPath)
                            ]
                        + SHorizontalBox::Slot()
                            .AutoWidth()
                            .Padding(2, 0, 0, 0)
                            .VAlign(VAlign_Center)
                            [
                                SNew(SButton)
                                .Text(FText::FromString("CREATE INITIAL CSV"))
                                .OnClicked(this, &SMatTabPanel::OnGenerateCSV)
                                .IsEnabled(this, &SMatTabPanel::CanGenerateCSV)
                                .ToolTipText(FText::FromString("Generate CSV from datasmith materials"))
                            ]
                    ]
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0.f, 5.f, 0.f, 0.f)
                        [
                            SNew(SHorizontalBox)
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            .Padding(2, 0, 0, 0)
                            .VAlign(VAlign_Center)
                                [
                                    SNew(STextBlock)
                                    .Text(FText::FromString(TEXT("Change matched materials:   ")))
                                ]
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            .Padding(2, 0, 0, 0)
                            .VAlign(VAlign_Center)
                                [
                                    SNew(SButton)
                                    .Text(FText::FromString("CHANGE MATERIALS"))
                                    .OnClicked(this, &SMatTabPanel::OnChangeMat)
                                    .IsEnabled(this, &SMatTabPanel::CanChangeMat)
                                    .ToolTipText(FText::FromString("Change materials from the MaxMats \ntable in MatComparer folder."))
                                ]
                    ]
                ]
            
                /*+ SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString("choose mat"))
                    ]
                + SHorizontalBox::Slot()
                    .FillWidth(0.5f)
                    [
                        AssetPickerWidget
                    ]
                + SHorizontalBox::Slot()
                    .FillWidth(0.5f)
                    [
                        SNullWidget::NullWidget
                    ]*/    
               ]
               
                   
        ];
}

TSharedRef<ITableRow> MakeListViewWidget(TSharedRef<FText> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
    return
        SNew(STableRow< TSharedRef<FText> >, OwnerTable)
        [
            SNew(STextBlock).Text(Item.Get())
        ];
}
void SMatTabPanel::SetCSVPath(const FString& Directory) {
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (PlatformFile.DirectoryExists(*Directory)) {
        SMatTabPanel::CSVsavePath = Directory;
        SMatTabPanel::isCSVPathValid = true;
    }else const EAppReturnType::Type Choice = FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString(TEXT("Wrong Folder")));
}

FReply SMatTabPanel::OnGenerateCSV() {
    
    SMatTabPanel::LoadData();
    EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString(TEXT("(WIP) This action will reset the MaxMats table with no material matches. \n To save table changes export MaxMats table as a CSV. \n\n Do you want to continue?")));
    if (ReturnType == EAppReturnType::Yes)
    {
        MatComparer.GenerateCSVwMaxMaterials(SMatTabPanel::CSVsavePath, MatComparer.AssetMats);
    }

    return FReply::Handled();
}

FReply SMatTabPanel::OnChangeMat()
{   
    SMatTabPanel::LoadData();
    EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString(TEXT("Are you sure you want to swap all scene materials?")));
    if (ReturnType == EAppReturnType::Yes)
    {
        MatComparer.SwapMaterials();
    }

    // Suggestions
    //if (!UEmaterialMatch) {
    //    MatComparer.GetUEMatSuggestions(realUEmat, MatComparer.UnrealMats);
    //    //TODO MATCH UNREALMAT WITH REAL UNREAL MAT TO SET NEW MATERIAL

    //} 

    return FReply::Handled();
}

FString SMatTabPanel::GetInitialPath() {
    return FPaths::ProjectContentDir() + "Datasmith/";
}

FString SMatTabPanel::GetGeometriesPath() {
    return "/Game/Datasmith/" + SMatTabPanel::sceneFolderName + "/Geometries";
}

FString SMatTabPanel::GetMaterialsPath() {
    return "/Game/Datasmith/" + SMatTabPanel::sceneFolderName + "/Materials";
}

FString SMatTabPanel::GetUnrealLibraryPath() {
    return "/Game/Datasmith/MatComparer/UnrealMatsLib";
}

bool SMatTabPanel::CanGenerateCSV() const {
    return SMatTabPanel::isSceneFolderValid && SMatTabPanel::isCSVPathValid;
}

bool SMatTabPanel::CanChangeMat() const
{
   /* FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    if (AssetRegistryModule.Get().IsLoadingAssets())
    {
        StartAsyncWork(LOCTEXT("WaitingForAssetRegistry", "Waiting for Asset Registry to finish loading"));
        bWaitingForAssetRegistryLoad = true;
    }
    else
    {
        SetupAssetRegistryCallbacks();
        BuildBasicMaterialTree();
    }*/    

    return SMatTabPanel::isSceneFolderValid;
}

void SMatTabPanel::LoadData() {
    MatComparer.AssetMeshes = MatComparer.GetDatasmithGeometries(FName(SMatTabPanel::GetGeometriesPath()));
    MatComparer.AssetMats = MatComparer.GetDatasmithMaterials(FName(SMatTabPanel::GetMaterialsPath()));
    MatComparer.UnrealMats = MatComparer.GetUEMaterials();
    MatComparer.RealUnrealMats = MatComparer.GetDatasmithMaterials(FName(SMatTabPanel::GetUnrealLibraryPath()));
}


void SMatTabPanel::SetCurrentFolderPath(const FString& Directory) {
    
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (PlatformFile.DirectoryExists(*Directory) && Directory != FPaths::ConvertRelativePathToFull(SMatTabPanel::GetInitialPath()))
    {
        FString RightStr;
        Directory.Split(TEXT("/"), NULL, &RightStr, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

        FString GeometriesFullPath = FPaths::ConvertRelativePathToFull(SMatTabPanel::GetInitialPath() + RightStr + "/Geometries");
        FString MaterialsFullPath = FPaths::ConvertRelativePathToFull(SMatTabPanel::GetInitialPath() + RightStr + "/Materials");
        if (PlatformFile.DirectoryExists(*GeometriesFullPath) && PlatformFile.DirectoryExists(*MaterialsFullPath)) {
            SMatTabPanel::sceneFolderName = RightStr;   
            SMatTabPanel::LoadData();
            SMatTabPanel::isSceneFolderValid = true;
            //UGameInstance* GameInstanceRef = Cast<UGameInstance >(GEngine->GetWorld()->GetGameInstance());

        }
        else {
            const EAppReturnType::Type Choice = FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString(TEXT("Wrong Folder")));
            SMatTabPanel::isSceneFolderValid = false;
        }
    }
}

#undef LOCTEXT_NAMESPACE
