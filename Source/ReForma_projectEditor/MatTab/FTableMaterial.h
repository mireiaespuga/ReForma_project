#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "FTableMaterial.generated.h"


USTRUCT(BlueprintType)
struct FTableMaterial : public FTableRowBase
{
    GENERATED_BODY()

public:

    FTableMaterial() {};

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



