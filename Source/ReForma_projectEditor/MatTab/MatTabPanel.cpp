#include "MatTabPanel.h"
#include "ReForma_projectEditor/ReForma_projectEditor.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "PropertyCustomizationHelpers.h"
#include "Engine/UserDefinedStruct.h"
#include "Dom/JsonObject.h"
#include "Misc/Paths.h"
#include "SlateBasics.h"


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
                            .Directory(SMatTabPanel::GetFolderPath())
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
               //Creating the button that adds a new item on the list when pressed
                   + SScrollBox::Slot()
                   [
                       SNew(SButton)
                       .Text(FText::FromString("Add new list item"))
                       .OnClicked(this, &SMatTabPanel::ButtonPressed)
                   ]
             
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
                       SNew(SButton)
                       .Text(FText::FromString("Add new list item"))
                       .OnClicked(this, &SMatTabPanel::ButtonPressed)
                   ]
                    [
                        SAssignNew(ListViewWidget, SListView<TSharedPtr<FMatItem>>)
                        .ItemHeight(24)
                        .ListItemsSource(&Items) //The Items array is the source of this listview
                        .OnGenerateRow(this, &SMatTabPanel::OnGenerateRowForList)
                    ]
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

//TSharedRef<ITableRow> MakeTileViewWidget(TSharedPtr<FAssetViewItem> AssetItem, const TSharedRef<STableViewBase>& OwnerTable)
//{
//    if (!ensure(AssetItem.IsValid()))
//    {
//        return SNew(STableRow<TSharedPtr<FAssetViewItem>>, OwnerTable);
//    }
//
//    VisibleItems.Add(AssetItem);
//    bPendingUpdateThumbnails = true;
//
//    if (AssetItem->IsFolder())
//    {
//        TSharedPtr< STableRow<TSharedPtr<FAssetViewItem>> > TableRowWidget;
//        SAssignNew(TableRowWidget, STableRow<TSharedPtr<FAssetViewItem>>, OwnerTable)
//            .Style(FEditorStyle::Get(), "ContentBrowser.AssetListView.TableRow")
//            .Cursor(bAllowDragging ? EMouseCursor::GrabHand : EMouseCursor::Default)
//            .OnDragDetected(this, &SAssetView::OnDraggingAssetItem);
//
//        TSharedRef<SAssetTileItem> Item =
//            SNew(SAssetTileItem)
//            .AssetItem(AssetItem)
//            .ItemWidth(this, &SAssetView::GetTileViewItemWidth)
//            .OnRenameBegin(this, &SAssetView::AssetRenameBegin)
//            .OnRenameCommit(this, &SAssetView::AssetRenameCommit)
//            .OnVerifyRenameCommit(this, &SAssetView::AssetVerifyRenameCommit)
//            .OnItemDestroyed(this, &SAssetView::AssetItemWidgetDestroyed)
//            .ShouldAllowToolTip(this, &SAssetView::ShouldAllowToolTips)
//            .HighlightText(HighlightedText)
//            .IsSelected(FIsSelected::CreateSP(TableRowWidget.Get(), &STableRow<TSharedPtr<FAssetViewItem>>::IsSelectedExclusively));
//
//        TableRowWidget->SetContent(Item);
//
//        return TableRowWidget.ToSharedRef();
//    }
//    else
//    {
//        TSharedPtr<FAssetThumbnail>& AssetThumbnail = RelevantThumbnails.FindOrAdd(AssetItem);
//        if (!AssetThumbnail)
//        {
//            const float ThumbnailResolution = TileViewThumbnailResolution;
//            AssetThumbnail = MakeShared<FAssetThumbnail>(FAssetData(), ThumbnailResolution, ThumbnailResolution, AssetThumbnailPool);
//            AssetItem->GetItem().UpdateThumbnail(*AssetThumbnail);
//            AssetThumbnail->GetViewportRenderTargetTexture(); // Access the texture once to trigger it to render
//        }
//
//        TSharedPtr< STableRow<TSharedPtr<FAssetViewItem>> > TableRowWidget;
//        SAssignNew(TableRowWidget, STableRow<TSharedPtr<FAssetViewItem>>, OwnerTable)
//            .Style(FEditorStyle::Get(), "ContentBrowser.AssetListView.TableRow")
//            .Cursor(bAllowDragging ? EMouseCursor::GrabHand : EMouseCursor::Default)
//            .OnDragDetected(this, &SAssetView::OnDraggingAssetItem);
//
//        TSharedRef<SAssetTileItem> Item =
//            SNew(SAssetTileItem)
//            .AssetThumbnail(AssetThumbnail)
//            .AssetItem(AssetItem)
//            .ThumbnailPadding(TileViewThumbnailPadding)
//            .ItemWidth(this, &SAssetView::GetTileViewItemWidth)
//            .OnRenameBegin(this, &SAssetView::AssetRenameBegin)
//            .OnRenameCommit(this, &SAssetView::AssetRenameCommit)
//            .OnVerifyRenameCommit(this, &SAssetView::AssetVerifyRenameCommit)
//            .OnItemDestroyed(this, &SAssetView::AssetItemWidgetDestroyed)
//            .ShouldAllowToolTip(this, &SAssetView::ShouldAllowToolTips)
//            .HighlightText(HighlightedText)
//            .ThumbnailEditMode(this, &SAssetView::IsThumbnailEditMode)
//            .ThumbnailLabel(ThumbnailLabel)
//            .ThumbnailHintColorAndOpacity(this, &SAssetView::GetThumbnailHintColorAndOpacity)
//            .AllowThumbnailHintLabel(AllowThumbnailHintLabel)
//            .IsSelected(FIsSelected::CreateSP(TableRowWidget.Get(), &STableRow<TSharedPtr<FAssetViewItem>>::IsSelectedExclusively))
//            .OnIsAssetValidForCustomToolTip(OnIsAssetValidForCustomToolTip)
//            .OnGetCustomAssetToolTip(OnGetCustomAssetToolTip)
//            .OnVisualizeAssetToolTip(OnVisualizeAssetToolTip)
//            .OnAssetToolTipClosing(OnAssetToolTipClosing);
//
//        TableRowWidget->SetContent(Item);
//
//        return TableRowWidget.ToSharedRef();
//    }
//}

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
            FReForma_projectEditor::Get().setFolderName(RightStr);
            SMatTabPanel::LoadData();
        }
        else {
            const EAppReturnType::Type Choice = FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString(TEXT("Wrong Folder")));
        }
    }
}


FReply SMatTabPanel::ButtonPressed()
{
    for (auto uetablemat : MatComparer.UnrealMats) {
        //Adds a new item to the array (do whatever you want with this)
        //FString* str = new FString(utabmat->MaterialName.ToString() + "    with    " + (utabmat->UMaterialMatch.ToString()));
  
        TArray<UMaterialInterface*> assetmat = MatComparer.AssetMats.FilterByPredicate([&](const UMaterialInterface* assetMat) {
            return assetMat->GetFName() == uetablemat->MaterialName;
            });

        if (assetmat.Num() > 0) {
           
            UMaterialInterface* mat = assetmat.Pop();

           /* UTexture2D* CurrentObject = mat.tex*/
            FAssetData AssetData = FAssetData(mat);
            const uint32 ThumbnailResolution = 64;
            TSharedPtr<FAssetThumbnail> Thumbnail = MakeShareable(new FAssetThumbnail(AssetData, ThumbnailResolution, ThumbnailResolution, ThumbnailPool));
            TSharedPtr<FMatItem> NewItem = MakeShareable(new FMatItem(mat->GetPathName(), Thumbnail));
        
            Items.Add(NewItem);

           // Update the listview
            ListViewWidget->RequestListRefresh();
        } 

    }
 

    return FReply::Handled();
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

    FAssetThumbnailConfig ThumbnailConfig;
    ThumbnailConfig.bAllowFadeIn = true;
    //Create the row
    //FString str = *Item.Get()->MaterialName.ToString().Append(*Item.Get()->UMaterialMatch.ToString());
    //return
    //    SNew(STableRow< TSharedPtr<FString>>, OwnerTable)
    //    .Padding(2.0f)
    //    [
    //        SNew(STextBlock)
    //        .Text(FText::FromString(*str))
    //    ]; //TODO: THUMBNAIL???????

    return SNew(STableRow<TSharedPtr<FMatItem>>, OwnerTable)
            .Style(FEditorStyle::Get(), "ContentBrowser.AssetListView.TableRow")
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
                            .Text(FText::FromName(ListAssetData.AssetClass))
                        ]
                    ]
        ];
    
}

#undef LOCTEXT_NAMESPACE
