#pragma once
#include  "mysql.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Docking/SDockableTab.h"
#include "Widgets/Docking/SDockTabStack.h"
#include "Framework/Application/SlateApplication.h"
#include "MatTab.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "Containers/Map.h"
#include "CoreMinimal.h"
#include "DataTableEditorUtils.h"
#include "FTableMaterial.h"
#include "../../../Plugins/MySQLConnectorUE4Plugin/Source/MySQLConnectorUE4Plugin/Public/MySQLDatabase.h"
#include "../DBTab/DBTab.h"

struct UEMatComparer {
    FName MaterialName;
    FName UMaterialMatch;
    TMap <FString, FName> TextureNames;
    FName FatherName;
    TMap <FString, float> ScalarValueParams;
    TMap <FString, FLinearColor> VectorValueParams;
    bool isMasterEntry;

    UEMatComparer() :
        MaterialName(TEXT(""))
    {}
};


class FMatComparer : public SCompoundWidget
{ 

public:

    TArray<UMaterialInterface*> AssetMats;
    TArray<UEMatComparer*> DictionaryMats;
    TArray<UEMatComparer*> SceneMats;
    TArray<UMaterialInterface*> RealUnrealMats;
    TArray<UStaticMesh*> AssetMeshes;
    UDataTable* DictionaryTable; 

    virtual UEMatComparer* GetUeMatMatch(UMaterialInterface* realuemat, TArray<UEMatComparer*>& maxmats);
    virtual void SetUeMatMatch(UMaterialInterface* realuemat, UEMatComparer*& maxmatmatch);
    virtual void SwapMaterials();
    virtual TArray<UEMatComparer*> GetUEMatSuggestions(UMaterialInterface* realuemat, TArray<UEMatComparer*> mats);
    
    virtual TArray<UEMatComparer*> GetUEMaterials(const FString type, bool bCanDelete = false);
    virtual void initDB();
    virtual int GetLastRowIndex(UDataTable* table);
    virtual TArray<UStaticMesh*> GetDatasmithGeometries(FName Path);
    virtual TArray<UMaterialInterface*> GetDatasmithMaterials(FName Path);
    virtual TArray<UDataTable*> GetDataTables(FName Path);
    virtual void ParseString(FString text, const FString type, UEMatComparer& uemat);
    virtual void GenerateCSVwMaxMaterials(FString SavePath, TArray<UMaterialInterface*> AssetMats);
    virtual FString MaxMatToFTableMat(UMaterialInterface* maxmat, int it, FTableMaterial*& intablemat);
    virtual bool AddMaterialToDict(UMaterialInterface* assetToImport);

    virtual bool MatNameCheck(UMaterialInterface& maxmat, UEMatComparer& uemat);
    virtual float TextureCheck(UMaterialInterface& maxmat, UEMatComparer& uemat, bool exactMatch = false);
    virtual float FatherCheck(UMaterialInterface& maxmat, UEMatComparer& uemat);
    virtual float ScalarParamsCheck(UMaterialInterface& maxmat, UEMatComparer& uemat, const float matchWeight, bool exactMatch = false);
    virtual float VectorParamsCheck(UMaterialInterface& maxmat, UEMatComparer& uemat, const float matchWeight, bool exactMatch = false);
    virtual bool AcceptSuggestion(UMaterialInterface* realuemat, UEMatComparer* matchmat, float threshold);
}; 
