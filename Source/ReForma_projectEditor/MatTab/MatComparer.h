#pragma once

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


struct UEMatComparer {
    FName MaterialName;
    FName UMaterialMatch;
    TMap <FString, FName> TextureNames;
    FName FatherName;
    TMap <FString, float> ScalarValueParams;
    TMap <FString, FLinearColor> VectorValueParams;

    UEMatComparer() :
        MaterialName(TEXT(""))
    {}
};


class FMatComparer : public SCompoundWidget
{ 

public:
    TArray<UMaterialInterface*> AssetMats;
    TArray<UEMatComparer*> UnrealMats;
    TArray<UMaterialInterface*> RealUnrealMats;
    TArray<UStaticMesh*> AssetMeshes;
    UDataTable* MaxMatsTable; 

    virtual UEMatComparer* GetUeMatMatch(UMaterialInterface* realuemat, TArray<UEMatComparer*>& maxmats);
    virtual void SetUeMatMatch(UMaterialInterface* realuemat, UEMatComparer*& maxmatmatch);
    virtual void SwapMaterials();
    virtual void GetUEMatSuggestions(UMaterialInterface* maxmat, TArray<UEMatComparer*>& UnrealMats);
    
    virtual TArray<UEMatComparer*> GetUEMaterials();
    virtual TArray<UStaticMesh*> GetDatasmithGeometries(FName Path);
    virtual TArray<UMaterialInterface*> GetDatasmithMaterials(FName Path);
    virtual TArray<UDataTable*> GetDataTables(FName Path);
    virtual void ParseString(FString text, const FString type, UEMatComparer& uemat);
    virtual void GenerateCSVwMaxMaterials(FString SavePath, TArray<UMaterialInterface*> AssetMats);
    virtual FString MaxMatToFTableMat(UMaterialInterface* maxmat, int it);

    virtual bool MatNameCheck(UMaterialInterface& maxmat, UEMatComparer& uemat);
    virtual float TextureCheck(UMaterialInterface& maxmat, UEMatComparer& uemat, bool exactMatch = false);
    virtual float FatherCheck(UMaterialInterface& maxmat, UEMatComparer& uemat);
    virtual float ScalarParamsCheck(UMaterialInterface& maxmat, UEMatComparer& uemat, const float matchWeight, bool exactMatch = false);
    virtual float VectorParamsCheck(UMaterialInterface& maxmat, UEMatComparer& uemat, const float matchWeight, bool exactMatch = false);

}; 
