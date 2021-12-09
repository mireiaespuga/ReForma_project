#pragma once

#include "ReForma_projectEditor/CustomTabToolBase.h"
#include "../MatTab/FTableMaterial.h"

class DBTab : public FCustomTabToolBase
{
    
    virtual void OnStartupModule() override;
    virtual void OnShutdownModule() override;
    virtual void MakeMenuEntry_(FMenuBuilder& menuBuilder) ;
    
public:
    static void InstertIntoDB(FString tablename, int newname, FTableMaterial* tablemat);
    static void loadArtistDB(UDataTable*& UETable);
    static void loadMasterDB(UDataTable*& UETable);
    static FString CreateArtistTable();
    static int GetLastDictEntry(FString tablename);
    
protected: 
    TSharedPtr<FUICommandList> CommandList = NULL;
    // UI Command functions
    void MapCommands();
    void ReloadDBCommand();

};

