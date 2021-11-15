#pragma once
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Docking/SDockableTab.h"
#include "Widgets/Docking/SDockTabStack.h"
#include "DesktopWidgets/Public/Widgets/Input/SDirectoryPicker.h"
//#include "DesktopPlatformModule.h"
#include "Framework/Application/SlateApplication.h"
#include "MatTab.h"
#include "MatComparer.h"
#include "Engine/DataTable.h"
#include "Containers/Map.h"
#include "CoreMinimal.h"
#include "EngineMinimal.h"


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
        void LoadData();
        bool isSceneFolderValid() const;
        
public: 
    
    FString CSVsavePath;
    FString MaxMatsTablePath = "DataTable'/Game/Datasmith/MatComparer/MaxMats.MaxMats'";

    bool isCSVPathValid = false;

protected:

    TWeakPtr<MatTab> tool;
    FMatComparer MatComparer;
   
};

