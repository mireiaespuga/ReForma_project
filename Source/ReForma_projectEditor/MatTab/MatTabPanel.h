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
    TSharedPtr<class FAssetThumbnail> Thumbnail;
    FString MatchObjectPath;
    TSharedPtr<class FAssetThumbnail> MatchThumbnail;
    bool isExactMatch;

    FMatItem(const FString& InObjectPath, const TSharedPtr<class FAssetThumbnail>& InThumbnail, const FString& InMatchObjectPath, const TSharedPtr<class FAssetThumbnail>& InMatchThumbnail, const bool &inIsExactMatch)
        : ObjectPath(InObjectPath)
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
        FReply OnGenerateCSV();    // because it is going to be used for a button, return type have to be FReply.
        bool CanGenerateCSV() const;
        void SetCurrentFolderPath(const FString& Directory);
        void SetCSVPath(const FString& Directory);
        FString GetInitialPath();
        FString GetFolderPath();
        FString GetGeometriesPath();
        FString GetMaterialsPath();
        FString GetUnrealLibraryPath();
        FString GetTypeOfMatch(TSharedPtr<FMatItem> Item);
        void LoadData();
        bool isSceneFolderValid() const;
       
        FReply ButtonPressed();

        /* Adds a new textbox with the string to the list */
        TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FMatItem> Item, const TSharedRef<STableViewBase>& OwnerTable);

public: 
    
    FString CSVsavePath;
    FString MaxMatsTablePath = "DataTable'/Game/Datasmith/MatComparer/MaxMats.MaxMats'";

    bool isCSVPathValid = false;

    TArray<TSharedPtr<FMatItem>> Items;

    /* The actual UI list */
    TSharedPtr<SListView<TSharedPtr<FMatItem>>> ListViewWidget;
    TSharedPtr<class FAssetThumbnailPool> ThumbnailPool = MakeShareable(new FAssetThumbnailPool(1024, false));

protected:

    TWeakPtr<MatTab> tool;
    FMatComparer MatComparer;
   
};

