#pragma once

#include "ReForma_projectEditor/ReForma_projectEditor.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "FTableMaterial.generated.h"


USTRUCT(BlueprintType)
struct FTableMaterial : public FTableRowBase
{
    GENERATED_BODY()


public:

    #if WITH_EDITOR
    virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override {
        FReForma_projectEditor::Get().CallSaveArtistDB(InDataTable, InRowName);
    };
    #endif

    FTableMaterial() {};
    UPROPERTY(BlueprintReadWrite)
        bool isMasterDictEntry = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString MaterialName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString UMaterialMatch;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString FatherName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString TextureNames;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString ScalarParamValues;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString VectorParamValues;
   

};



