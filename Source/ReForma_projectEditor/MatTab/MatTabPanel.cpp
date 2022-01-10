#include "MatTabPanel.h"
#include "ReForma_projectEditor/ReForma_projectEditor.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "PropertyCustomizationHelpers.h"
#include "Engine/UserDefinedStruct.h"
#include "Dom/JsonObject.h"
#include "Misc/Paths.h"
#include "SlateBasics.h"
#include "ThumbnailRendering/ThumbnailManager.h"


#define LOCTEXT_NAMESPACE "SMatTabPanel"

void SMatTabPanel::Construct(const FArguments& InArgs)
{
    tool = InArgs._Tool;
    if (tool.IsValid())
    {
        // do anything you need from tool object
    }

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
                            .Directory(SMatTabPanel::GetFolderPath())
                            .OnDirectoryChanged(this, &SMatTabPanel::SetCurrentFolderPath)
                        ]
                    ]
                  /*   + SVerticalBox::Slot()
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
                    ]*/
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
              
               + SScrollBox::Slot()
                .VAlign(VAlign_Top)
                .Padding(5)
                [
                    SNew(SBorder)
                    .BorderBackgroundColor(FColor(192, 192, 192, 255))
                    .Padding(15.0f)
                   [
                       SNew(SVerticalBox) 
                       + SVerticalBox::Slot()
                       .AutoHeight()
                       .Padding(0.f, 0.f, 0.f, 4.f)
                       [
                           
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Visualize matched materials:   ")))
                           
                       ]
                          
                        + SVerticalBox::Slot()
                       .AutoHeight()
                       .Padding(0.f, 0.f, 0.f, 4.f)
                       [
                           SNew(SHorizontalBox)
                           + SHorizontalBox::Slot()
                           .AutoWidth()
                           .VAlign(VAlign_Center)
                           [
                               SNew(SButton)
                               .Text(FText::FromString("Exact Matches"))
                               .OnClicked(this, &SMatTabPanel::VisualizeButtonPressed)
                               .IsEnabled(this, &SMatTabPanel::CanChangeMat)
                               //.ButtonColorAndOpacity(FColor::Transparent)
                               //.ButtonStyle(this, &SItemWidget::GetItemIcon)
                           ]   
                           + SHorizontalBox::Slot()
                           .AutoWidth()
                           .VAlign(VAlign_Center)
                           .HAlign(HAlign_Right)
                           [
                               SNew(SButton)
                               .Text(FText::FromString("Get Suggestions"))
                               .OnClicked(this, &SMatTabPanel::FilterButtonPressed) 
                               .IsEnabled(this, &SMatTabPanel::canGetSuggestions)
                               //.ButtonColorAndOpacity(FColor::Transparent)
                               //.ButtonStyle(this, &SItemWidget::GetItemIcon)
                           ]
                           //+ SHorizontalBox::Slot()
                           // .AutoWidth()
                           // .VAlign(VAlign_Center)
                           // .HAlign(HAlign_Right)
                           // [
                           //     SNew(SNumericEntryBox<float>)
                           //     .ToolTipText(FText::FromString("Valid Suggestion threshold"))
                           //     .AllowSpin(true)
                           //     .OnValueChanged_Lambda([&](float InValue) { thr = InValue; })
                           //     .IsEnabled(this, &SMatTabPanel::CanChangeMat)
                           // ]
                       ]
                       + SVerticalBox::Slot()
                       .AutoHeight()
                        [
                            SAssignNew(ListViewWidget, SListView<TSharedPtr<FMatItem>>)
                            .ItemHeight(24)
                            .ListItemsSource(&DisplayedItems) //The Items array is the source of this listview
                            .OnGenerateRow(this, &SMatTabPanel::OnGenerateRowForList)
                            
                        ]
                   ]
                ]
                   
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

FString SMatTabPanel::GetFolderPath() {
    return  SMatTabPanel::isSceneFolderValid() ? SMatTabPanel::GetInitialPath() + FReForma_projectEditor::Get().GetFolderName() : SMatTabPanel::GetInitialPath();
}

FString SMatTabPanel::GetGeometriesPath() {
    return "/Game/Datasmith/" + FReForma_projectEditor::Get().GetFolderName() + "/Geometries";
}

FString SMatTabPanel::GetMaterialsPath() {
    return "/Game/Datasmith/" + FReForma_projectEditor::Get().GetFolderName() + "/Materials";
}

FString SMatTabPanel::GetUnrealLibraryPath() {
    return "/Game/Datasmith/MatComparer/UnrealMatsLib";
}

bool SMatTabPanel::CanGenerateCSV() const {
    return SMatTabPanel::isSceneFolderValid() && SMatTabPanel::isCSVPathValid;
}

bool SMatTabPanel::isSceneFolderValid() const {
    return FReForma_projectEditor::Get().GetFolderName() != "NONE_folderNoValid";
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
    return SMatTabPanel::isSceneFolderValid();
}

void SMatTabPanel::LoadData() {
    MatComparer.DictionaryMats = MatComparer.GetUEMaterials("DICTIONARY", true);
    MatComparer.AssetMeshes = MatComparer.GetDatasmithGeometries(FName(SMatTabPanel::GetGeometriesPath()));
    MatComparer.AssetMats = MatComparer.GetDatasmithMaterials(FName(SMatTabPanel::GetMaterialsPath()));
    
    MatComparer.SceneMats = MatComparer.GetUEMaterials("SCENE");
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
            FReForma_projectEditor::Get().setFolderName(RightStr);
        }
        else {
            const EAppReturnType::Type Choice = FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString(TEXT("Wrong Folder")));
        }
    }
}

FReply SMatTabPanel::VisualizeButtonPressed()
{
    SMatTabPanel::LoadData();
    
    DisplayedItems.Empty();
    ExactItems.Empty();
    SuggestedItems.Empty();

    for (auto uetablemat : MatComparer.SceneMats) {
        //Adds a new item to the array (do whatever you want with this)

        TArray<UMaterialInterface*> assetmat = MatComparer.AssetMats.FilterByPredicate([&](UMaterialInterface* assetMat) {
            return MatComparer.MatNameCheck(*assetMat, *uetablemat);
            });
        TArray<UMaterialInterface*> matchedmat = MatComparer.RealUnrealMats.FilterByPredicate([&](const UMaterialInterface* ulibMat) {
            return ulibMat->GetFName() == uetablemat->UMaterialMatch;
            });

        if (assetmat.Num() > 0) {
           
            UMaterialInterface* mat = assetmat.Pop();
            FAssetData AssetData = FAssetData(mat);  
            const uint32 ThumbnailResolution = 64;
            TSharedPtr<FAssetThumbnail> Thumbnail = MakeShareable(new FAssetThumbnail(AssetData, ThumbnailResolution, ThumbnailResolution, ThumbnailPool));


            if (matchedmat.Num() > 0) { //There's an exact match from dictionary
                UMaterialInterface* matchmat = matchedmat.Pop();
                FAssetData MatchedAssetData = FAssetData(matchmat);
                TSharedPtr<FAssetThumbnail> matchedThumbnail = MakeShareable(new FAssetThumbnail(MatchedAssetData, ThumbnailResolution, ThumbnailResolution, ThumbnailPool));
                
                TSharedPtr<FMatItem> NewItem = MakeShareable(new FMatItem(mat->GetPathName(), mat->GetFName().ToString(), Thumbnail, matchmat->GetPathName(), matchedThumbnail, true));
                ExactItems.Add(NewItem);
            }
            else { //no dictionary entry

                //Get suggestions for closest maxmats materials in dictionnary
                TArray<UEMatComparer*> suggestions = MatComparer.GetUEMatSuggestions(mat, MatComparer.DictionaryMats);

                //Find the matched unreal material related to the first suggestion
                if (suggestions.Num() > 0) {
                    UEMatComparer* firstsuggest = suggestions[0];
                    TArray<UMaterialInterface*> suggestedmat = MatComparer.RealUnrealMats.FilterByPredicate([&](const UMaterialInterface* ulibMat) {
                        return ulibMat->GetFName() == firstsuggest->UMaterialMatch;
                        });

                    UMaterialInterface* matchmat = nullptr;
                    if (suggestedmat.Num() > 0 && MatComparer.AcceptSuggestion(mat, firstsuggest, thr)) {
                        matchmat = suggestedmat.Pop();
                    }
                    else { //if first suggestion is not found
                        for (auto othersugg : suggestions) {
                            TArray<UMaterialInterface*> suggestedmats = MatComparer.RealUnrealMats.FilterByPredicate([&](const UMaterialInterface* ulibMat) {  
                                return ulibMat->GetFName() == othersugg->UMaterialMatch;
                                });
                            if (suggestedmats.Num() > 0 && MatComparer.AcceptSuggestion(mat, othersugg, thr)) {
                                matchmat = suggestedmats.Pop();
                                break;
                            }
                        }
                    }

                    if (matchmat) {
                        FAssetData MatchedAssetData = FAssetData(matchmat);
                        TSharedPtr<FAssetThumbnail> matchedThumbnail = MakeShareable(new FAssetThumbnail(MatchedAssetData, ThumbnailResolution, ThumbnailResolution, ThumbnailPool));
                        TSharedPtr<FMatItem> NewItem = MakeShareable(new FMatItem(mat->GetPathName(), mat->GetFName().ToString(), Thumbnail, matchmat->GetPathName(), matchedThumbnail, false));
                        SuggestedItems.Add(NewItem);
                    }   
                }

            }
            
        } 

    }
    SMatTabPanel::bGetSuggestions = true;
    //we only see exact matches
    DisplayedItems = ExactItems;

    // Update the listview
    ListViewWidget->RequestListRefresh();
    
    return FReply::Handled();
}


FReply SMatTabPanel::FilterButtonPressed() {
    SMatTabPanel::LoadData();
    DisplayedItems = SuggestedItems;

    // Update the listview
    ListViewWidget->RequestListRefresh();
    return FReply::Handled();
}

//TSharedRef<ITableRow> SMatTabPanel::OnGenerateSuggestions(TSharedPtr<FMatItem> Item, const TSharedRef<STableViewBase>& OwnerTable){
//
//}

TSharedRef <SButton> SMatTabPanel::FinderOrAddButton(TSharedPtr<FMatItem> Item, const TSharedRef<STableViewBase>& OwnerTable) {

    if(Item->isExactMatch){
        return SNew(SButton)
            .ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
            .ToolTipText(Item->isExactMatch ? FText::FromString("Find match in table") : FText::FromString("Not enabled"))
            .ContentPadding(4.0f)
            .IsEnabled(Item->isExactMatch)
            .OnClicked(this, &SMatTabPanel::OpenTable, Item)
            .ForegroundColor(FSlateColor::UseForeground())
            [
                SNew(SImage)
                .Image(FEditorStyle::GetBrush("PropertyWindow.Button_Browse"))
            ];
    }
    else {
        return SNew(SButton)
            .ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
            .ToolTipText(!Item->isExactMatch ? FText::FromString("Add match to table") : FText::FromString("Not enabled"))
            .ContentPadding(4.0f)
            .IsEnabled(!Item->isExactMatch)
            .OnClicked(this, &SMatTabPanel::AddEntry, Item)
            .ForegroundColor(FSlateColor::UseForeground())
            [
                SNew(SImage)
                .Image(FEditorStyle::GetBrush("PropertyWindow.Button_Edit"))
            ];
    }
}



TSharedRef<ITableRow> SMatTabPanel::OnGenerateRowForList(TSharedPtr<FMatItem> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
    if (!ensure(Item.IsValid()))
    {
        return SNew(STableRow<TSharedPtr<FMatItem>>, OwnerTable);
    }

    const uint32 ThumbnailResolution = 64;
    const uint32 ThumbnailBoxPadding = 4;
    UObject* ListObject = FindObject<UObject>(NULL, *Item->ObjectPath);
    FAssetData ListAssetData(ListObject);
    
    UObject* MatchedListObject = FindObject<UObject>(NULL, *Item->MatchObjectPath);
    FAssetData MatchedListAssetData(MatchedListObject);

    FAssetThumbnailConfig ThumbnailConfig;
    ThumbnailConfig.bAllowFadeIn = true;

    return SNew(STableRow<TSharedPtr<FMatItem>>, OwnerTable)
            .Style(FEditorStyle::Get(), "ContentBrowser.AssetListView.TableRow")
            
            [
                SNew(SUniformGridPanel)
                .SlotPadding(2)
                
                + SUniformGridPanel::Slot(0, 0)
                [
                    SNew(SHorizontalBox)
                    // Viewport
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    
                    [
                        SNew(SBox)
                        .WidthOverride(ThumbnailResolution + ThumbnailBoxPadding * 2)
                        .HeightOverride(ThumbnailResolution + ThumbnailBoxPadding * 2)
                        [
                            // Drop shadow border
                            SNew(SBorder)
                            .Padding(ThumbnailBoxPadding)
                            .BorderImage(FEditorStyle::GetBrush("ContentBrowser.ThumbnailShadow"))
                            [
                                Item->Thumbnail->MakeThumbnailWidget(ThumbnailConfig)
                            ]
                        ]
                    ]
                    + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(6, 0, 0, 0)
                        .VAlign(VAlign_Center)
                        [
                            SNew(SVerticalBox)
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(0, 1)
                            [
                                SNew(STextBlock)
                                .Font(FEditorStyle::GetFontStyle("ContentBrowser.AssetTileViewNameFont"))
                                .Text(FText::FromName(ListAssetData.AssetName))
                            ]

                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(0, 1)
                            [
                                // Class
                                SNew(STextBlock)
                                .Font(FEditorStyle::GetFontStyle("ContentBrowser.AssetListViewClassFont"))
                                .Text(FText::FromString("Datasmith Material"))
                            ]
                        ]
                ]
                + SUniformGridPanel::Slot(1, 0)
                [
                    SNew(SHorizontalBox)
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(10, 0, 0, 0)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SBox)
                        .WidthOverride(ThumbnailResolution + ThumbnailBoxPadding * 2)
                        .HeightOverride(ThumbnailResolution + ThumbnailBoxPadding * 2)
                        [
                            // Drop shadow border
                            SNew(SBorder)
                            .Padding(ThumbnailBoxPadding)
                            .BorderImage(FEditorStyle::GetBrush("ContentBrowser.ThumbnailShadow"))
                            [
                                 Item->MatchThumbnail->MakeThumbnailWidget(ThumbnailConfig)
                            ]
                        ]
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(6, 0, 0, 0)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SVerticalBox)
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 1)
                        [
                            SNew(STextBlock)
                            .Font(FEditorStyle::GetFontStyle("ContentBrowser.AssetTileViewNameFont"))
                            .Text(FText::FromName(MatchedListAssetData.AssetName))
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 1)
                        [
                            // Class
                            SNew(STextBlock)
                            .Font(FEditorStyle::GetFontStyle("ContentBrowser.AssetListViewClassFont"))
                            .Text(FText::FromString("UnrealLib Material"))
                        ]
                    ]
                ]
                + SUniformGridPanel::Slot(2, 0)
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(10, 0, 0, 0)
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .Font(FEditorStyle::GetFontStyle("ContentBrowser.AssetListViewClassFont"))
                        .Text(FText::FromString(SMatTabPanel::GetTypeOfMatch(Item)))
                    ]
                    + SHorizontalBox::Slot()
                    .FillWidth(1)
                    .Padding(0, 0, 20, 0)
                    .VAlign(VAlign_Center)
                    .HAlign(HAlign_Right)
                    [
                       
                        
                        SMatTabPanel::FinderOrAddButton(Item, OwnerTable)
                        
                        
                    //.ButtonStyle(this, &SItemWidget::GetItemIcon)
                    ]
                ]
                
         ];
    
}

FString SMatTabPanel::GetTypeOfMatch(TSharedPtr<FMatItem> Item) {
    return Item->isExactMatch ? "Exact Match" : "Suggestion";
}

FReply SMatTabPanel::OpenTable(TSharedPtr<FMatItem> Item) {

    for (auto it : MatComparer.DictionaryTable->GetRowMap()) {
        FTableMaterial* data = (FTableMaterial*)(it.Value);
        if (data->MaterialName == *Item->ObjectName) {
            FString context;
            FTableMaterial* row = MatComparer.DictionaryTable->FindRow<FTableMaterial>(it.Key, context);
            if (row) {
                GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(MatComparer.DictionaryTable);
                IAssetEditorInstance* ae = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(MatComparer.DictionaryTable, true);
                TSharedPtr< FTabManager > tabmanager = ae->GetAssociatedTabManager();
                TSharedRef< SDockTab > editortab = tabmanager->InvokeTab(FTabId("DataTableEditor_RowEditor"));
                editortab->DrawAttention();
                FDataTableEditorUtils::SelectRow(MatComparer.DictionaryTable, it.Key);
                return FReply::Handled();
            }
        }
    }
    
    return FReply::Handled();
}

FReply SMatTabPanel::AddEntry(TSharedPtr<FMatItem> Item) {
    
    UObject* ListObject = FindObject<UObject>(NULL, *Item->ObjectPath);
    FAssetData ListAssetData(ListObject);
    UMaterialInterface* test = Cast<UMaterialInterface>(ListAssetData.GetAsset());

    UObject* MatchObject = FindObject<UObject>(NULL, *Item->MatchObjectPath);
    FAssetData MatchAssetData(MatchObject);
    UMaterialInterface* match = Cast<UMaterialInterface>(MatchAssetData.GetAsset());
    
    MatComparer.AddMaterialToDict(test);
    MatComparer.SetUeMatMatch(test, match);
    SMatTabPanel::OpenTable(Item);
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
