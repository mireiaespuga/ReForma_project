#pragma once
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Docking/SDockableTab.h"
#include "Widgets/Docking/SDockTabStack.h"
#include "DesktopWidgets/Public/Widgets/Input/SDirectoryPicker.h"
#include "Framework/Application/SlateApplication.h"
#include "MatTab.h"
#include "MatComparer.h"
#include "Engine/DataTable.h"
#include "Containers/Map.h"
#include "CoreMinimal.h"
#include "EngineMinimal.h"
#include "Widgets/Views/SListView.h"

struct FMatItem
{
    FString ObjectPath;
    FString ObjectName;
    TSharedPtr<class FAssetThumbnail> Thumbnail;
    FString MatchObjectPath;
    TSharedPtr<class FAssetThumbnail> MatchThumbnail;
    bool isExactMatch;

    FMatItem(const FString& InObjectPath, const FString& InObjectName, const TSharedPtr<class FAssetThumbnail>& InThumbnail, const FString& InMatchObjectPath, const TSharedPtr<class FAssetThumbnail>& InMatchThumbnail, const bool& inIsExactMatch)
        : ObjectPath(InObjectPath)
        , ObjectName(InObjectName)
        , Thumbnail(InThumbnail)
        , MatchObjectPath(InMatchObjectPath)
        , MatchThumbnail(InMatchThumbnail)
        , isExactMatch(inIsExactMatch)
    {}
};


class SMatTabPanel : public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SMatTabPanel)
    {}
    SLATE_ARGUMENT(TWeakPtr<class MatTab>, Tool)
    
        SLATE_END_ARGS()

        void Construct(const FArguments& InArgs);
        FReply OnChangeMat();    // because it is going to be used for a button, return type have to be FReply.
        bool CanChangeMat() const;
        void SetCurrentFolderPath(const FString& Directory);
        
        FString GetInitialPath();
        FString GetFolderPath();
        FString GetGeometriesPath();
        FString GetMaterialsPath();
        FString GetUnrealLibraryPath();
        FString GetTypeOfMatch(TSharedPtr<FMatItem> Item);
        void LoadData();
        bool isSceneFolderValid() const;
        
        
        bool canGetSuggestions() const { return bGetSuggestions; };
        FReply VisualizeButtonPressed();
        FReply FilterButtonPressed();
        FReply OpenTable(TSharedPtr<FMatItem> Item);
        FReply AddEntry(TSharedPtr<FMatItem> Item);

        /* Adds a new textbox with the string to the list */
        TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FMatItem> Item, const TSharedRef<STableViewBase>& OwnerTable);
        FOnClicked OnDoubleClick(TSharedPtr<FMatItem> Item, const TSharedRef<STableViewBase>& OwnerTable);
        TSharedRef <SButton> FinderOrAddButton(TSharedPtr<FMatItem> Item, const TSharedRef<STableViewBase>& OwnerTable);

public: 
    
    FString MaxMatsTablePath = "DataTable'/Game/Datasmith/MatComparer/MaxMats.MaxMats'";

    TArray<TSharedPtr<FMatItem>> SuggestedItems;
    TArray<TSharedPtr<FMatItem>> ExactItems;
    TArray<TSharedPtr<FMatItem>> DisplayedItems;
    float thr = 4.0;
    bool bGetSuggestions = false;

    /* The actual UI list */
    TSharedPtr<SListView<TSharedPtr<FMatItem>>> ListViewWidget;
    TSharedPtr<class FAssetThumbnailPool> ThumbnailPool = MakeShareable(new FAssetThumbnailPool(1024, false));

protected:

    TWeakPtr<MatTab> tool;
    FMatComparer MatComparer;
   
};

