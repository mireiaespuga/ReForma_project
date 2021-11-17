#include "MatComparer.h"
#include "ReForma_projectEditor/ReForma_projectEditor.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "PropertyCustomizationHelpers.h"
#include "Engine/UserDefinedStruct.h"
#include "Dom/JsonObject.h"


#define LOCTEXT_NAMESPACE "FMatComparer"

TArray<UStaticMesh*> FMatComparer::GetDatasmithGeometries(FName Path) {
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    TArray<FAssetData> FoundAssets;
    FARFilter Filter;
    Filter.PackagePaths.Add(Path);  //TODO: make folder name editable
    AssetRegistryModule.Get().GetAssets(Filter, FoundAssets);

    TArray<UStaticMesh*> assets;
    for (auto Selected : FoundAssets) {
        auto* asset = Selected.GetAsset();
        UStaticMesh* test = Cast<UStaticMesh>(asset);
        assets.Push(test);
    }
    return assets;
}

TArray<UMaterialInterface*> FMatComparer::GetDatasmithMaterials(FName Path) {
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    TArray<FAssetData> FoundAssets;
    FARFilter Filter;
    Filter.PackagePaths.Add(Path);  //TODO: make folder name editable
    AssetRegistryModule.Get().GetAssets(Filter, FoundAssets);

    TArray<UMaterialInterface*> assets;

    for (auto Selected : FoundAssets) {
        auto* asset = Selected.GetAsset();
        UMaterialInterface* test = Cast<UMaterialInterface>(asset);
        assets.Push(test);
    }
    return assets;
}

void FMatComparer::ParseString(FString text, const FString type, UEMatComparer& uemat) {
    FString data, Right, auxtext = text;
    int it = 0;

    while (auxtext.Split(TEXT(";"), &data, &Right)) {
        FString dataId, dataValue;
        data.Split(TEXT("="), &dataId, &dataValue);

        if (type == "TEXTURES") {
            dataId = dataId + "_" + FString::FromInt(it);
            it++;
            uemat.TextureNames.Emplace(dataId, FName(dataValue));
            uemat.TextureNames.KeySort(TLess<FString>());
        }
        else if (type == "SCALAR") {
            float value = FCString::Atof(*dataValue);
            uemat.ScalarValueParams.Emplace(dataId, value);
            uemat.VectorValueParams.KeySort(TLess<FString>());
        }
        else if (type == "VECTOR") {
            FLinearColor color;
            dataValue = dataValue.Replace(TEXT("|"), TEXT(","));
            color.InitFromString(dataValue);
            uemat.VectorValueParams.Emplace(dataId, color);
            uemat.VectorValueParams.KeySort(TLess<FString>());
        }
        auxtext = Right;
    }

}

TArray < UDataTable*> FMatComparer::GetDataTables(FName Path) {
    // TODO: FDataTableImporterCSV::ReadTable()
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    TArray<FAssetData> FoundAssets;
    FARFilter Filter;
    //Filter.ClassNames.Add(UUserDefinedStruct::StaticClass()->GetFName());
    Filter.ClassNames.Add(UDataTable::StaticClass()->GetFName());
    Filter.PackagePaths.Add(Path);  //TODO: make folder name editable
    AssetRegistryModule.Get().GetAssets(Filter, FoundAssets);

    TArray<UDataTable*> DataTables;
    for (auto Selected : FoundAssets) {
        auto* asset = Selected.GetAsset();
        UDataTable* test = Cast<UDataTable>(asset);
        DataTables.Push(test);
    }
    return DataTables;
}

TArray<UEMatComparer*> FMatComparer::GetUEMaterials() {

    /*UDataTable* UETable = (FMatComparer::GetDataTables(Path).FilterByPredicate([](UDataTable* asset) {
        return asset->GetFName().ToString().Contains(TEXT("MaxMats"));
        }).Pop());*/ 
    UDataTable* UETable = LoadObject<UDataTable>(NULL, UTF8_TO_TCHAR("DataTable'/Game/Datasmith/MatComparer/MaxMats.MaxMats'"));
    FMatComparer::MaxMatsTable = UETable;
    TArray<UEMatComparer*> UEMats;
  
    for (auto it : UETable->GetRowMap())
    {
        // it.Key has the key from first column of the CSV file
        // it.Value has a pointer to a struct of data. You can safely cast it to your actual type, e.g FMyStruct* data = (FMyStruct*)(it.Value);
        FTableMaterial* data = (FTableMaterial*)(it.Value);
        UEMatComparer* uemat = new UEMatComparer();

        //start parsing strings into correct UEMatComparer materials
        uemat->MaterialName = FName(data->MaterialName);      
        uemat->FatherName = FName(data->FatherName);
        uemat->UMaterialMatch = FName(data->UMaterialMatch);

        FMatComparer::ParseString(data->TextureNames, "TEXTURES", *uemat);
        FMatComparer::ParseString(data->ScalarParamValues, "SCALAR", *uemat);
        FMatComparer::ParseString(data->VectorParamValues, "VECTOR", *uemat);

        UEMats.Push(uemat);
    }

    return UEMats;
}

//bool MaterialComparer(UMaterial& mat1, UMaterial& mat2) {
//
//    //UTexture* tex1, * tex2;
//    TArray<UTexture*> T1, T2;
//    bool areEqual = false;
//
//    mat1.GetUsedTextures(T1, EMaterialQualityLevel::High, true, ERHIFeatureLevel::SM5, true);
//    mat2.GetUsedTextures(T2, EMaterialQualityLevel::High, true, ERHIFeatureLevel::SM5, true);
//
//    //FTexture2DMipMap* MyMipMap = &MyTexture2D->PlatformData->Mips[0];
//    //https://answers.unrealengine.com/questions/25594/accessing-pixel-values-of-texture2d.html
//    
//}

bool FMatComparer::MatNameCheck(UMaterialInterface& realuemat, UEMatComparer& maxmat) {
    return realuemat.GetFName() == maxmat.MaterialName;
}

UEMatComparer* FMatComparer::GetUeMatMatch(UMaterialInterface* realMaxmat, TArray<UEMatComparer*>& maxmats) {

    TArray<UEMatComparer*> matches = maxmats.FilterByPredicate([&](UEMatComparer* maxmat) {
    return FMatComparer::MatNameCheck(*realMaxmat, *maxmat);
    }); //find exact match by name

    if (matches.Num() >0 ) {
        UEMatComparer* match = matches.Pop();
        // check exact paramaters match
        float m1match = FMatComparer::TextureCheck(*realMaxmat, *match, true);
        if (!FMatComparer::TextureCheck(*realMaxmat, *match, true)) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, match->MaterialName.ToString() + "  -->  " + TEXT("tex"));

        m1match += FMatComparer::FatherCheck(*realMaxmat, *match);
        if (!FMatComparer::FatherCheck(*realMaxmat, *match)) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, match->MaterialName.ToString() + "  -->  " + TEXT("father"));

        m1match += FMatComparer::ScalarParamsCheck(*realMaxmat, *match, 1.0, true);
        if (!FMatComparer::ScalarParamsCheck(*realMaxmat, *match, 1.0, true)) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, match->MaterialName.ToString() + "  -->  " + TEXT("sca"));

        m1match += FMatComparer::VectorParamsCheck(*realMaxmat, *match, 1.0, true);
        if (!FMatComparer::VectorParamsCheck(*realMaxmat, *match, 1.0, true)) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, match->MaterialName.ToString() + "  -->  " + TEXT("vec"));

        if (m1match == 4.0) return match; 
    } 
    return NULL;

}

float FMatComparer::TextureCheck(UMaterialInterface& realuemat, UEMatComparer& maxmat, bool exactMatch) {
    float matches = 0.0f;
    TArray<UTexture*> TexMax;
    TArray<FName> uetexNames;
    bool noText = false;

    realuemat.GetUsedTextures(TexMax, EMaterialQualityLevel::High, true, ERHIFeatureLevel::SM5, true);
    maxmat.TextureNames.GenerateValueArray(uetexNames);

    //check if both mats have same num of tex and have same name
    if (TexMax.Num() == maxmat.TextureNames.Num()) {
        
        // sort by name
        TexMax.Sort([](UTexture& realm1, UTexture& realm2) { return realm1.GetFName().LexicalLess(realm2.GetFName()); });
        uetexNames.Sort([](FName maxm1, FName maxm2) { return maxm1.LexicalLess(maxm2); });

        if (TexMax.Num() == 0) {
            matches++;  //both have no textures
            noText = true;
        }

        for (size_t i = 0; i < TexMax.Num(); i++) {
            matches = TexMax[i]->GetFName() == uetexNames[i] ? matches + 1 : matches; //share textures
        }
    }
    
    if (exactMatch) return FMath::FloorToInt(matches) == TexMax.Num() || noText ? 1.0 : 0.0;
    else return matches;
}

float FMatComparer::FatherCheck(UMaterialInterface& maxmat, UEMatComparer& uemat) {
    return maxmat.GetBaseMaterial()->GetFName() == uemat.FatherName ? 1.0 : 0.0;
}

float FMatComparer::ScalarParamsCheck(UMaterialInterface& maxmat, UEMatComparer& uemat, const float matchWeight, bool exactMatch) {
    float matches = 0.0f;

    //check for scalar parameters values
    TArray< FMaterialParameterInfo > ScalarParamsMax;
    TArray <FGuid> ScalarIdMax;
    maxmat.GetAllScalarParameterInfo(ScalarParamsMax, ScalarIdMax);
    float pmaxvalue;
    bool noScal = false;

    if (ScalarParamsMax.Num() == 0 && uemat.ScalarValueParams.Num() == 0) {
        noScal = true;
    }
    

    for (auto p : ScalarParamsMax) {
        //TODO: if (9) is irrelevant remove it at the end of p.Name. i.e Fresnel_IOR (9) -> Fresnel_IOR

        /*FString pName;
        p.Name.ToString().Split(TEXT(" ("), &pName, NULL);*/
        float pUevalue = uemat.ScalarValueParams.Contains(p.Name.ToString()) ? uemat.ScalarValueParams[p.Name.ToString()] : -1.0f;

        if (maxmat.GetScalarParameterValue(p, pmaxvalue)) {
            matches = pmaxvalue == pUevalue ? matches + matchWeight : matches; //have same scalar param value
        }

    }
    if (exactMatch) return matches == ScalarParamsMax.Num() * matchWeight || noScal ? 1.0 : 0.0;
    else return matches;
}

float FMatComparer::VectorParamsCheck(UMaterialInterface& maxmat, UEMatComparer& uemat, const float matchWeight, bool exactMatch) {
    float matches = 0.0f;

    TArray< FMaterialParameterInfo > VecParamsMax;
    TArray <FGuid> VecIdMax;
    maxmat.GetAllVectorParameterInfo(VecParamsMax, VecIdMax);
    FLinearColor pmaxvalue;
    bool noVect = false;

    if (VecParamsMax.Num() == 0 && uemat.VectorValueParams.Num() == 0) {
        noVect = true;
    }

    for (auto p : VecParamsMax) {
        //TODO: if (9) is irrelevant remove it at the end of p.Name. i.e Fresnel_IOR (9) -> Fresnel_IOR
        /*FString pName;
        p.Name.ToString().Split(TEXT(" ("), &pName, NULL);*/

        FLinearColor pUevalue = uemat.VectorValueParams.Contains(p.Name.ToString()) ? uemat.VectorValueParams[p.Name.ToString()] : FLinearColor(-1.0f, -1.0f, -1.0f, -1.0f);

        if (maxmat.GetVectorParameterValue(p, pmaxvalue)) {
            matches = pmaxvalue.Equals(pUevalue) ? matches + matchWeight : matches; //have same scalar param value
        }
    }

    if (exactMatch) return matches == VecParamsMax.Num() * matchWeight || noVect ? 1.0f : 0.0f;
    else return matches;

}

void FMatComparer::SetUeMatMatch(UMaterialInterface* realuemat, UEMatComparer*& maxmatmatch) {
    UDataTable* UETable = LoadObject<UDataTable>(NULL, UTF8_TO_TCHAR("DataTable'/Game/Datasmith/MatComparer/MaxMats.MaxMats'"));

    for (auto row : UETable->GetRowMap()) {
        // row.Key has the key from first column of the CSV file
        // row.Value has a pointer to a struct of data. You can safely cast it to your actual type, e.g FMyStruct* data = (FMyStruct*)(row.Value);
        FTableMaterial* data = (FTableMaterial*)(row.Value);
        if (data->MaterialName == maxmatmatch->MaterialName.ToString()) {
            data->UMaterialMatch = realuemat->GetFName().ToString();
            row.Value = (uint8*)data; //change row for modified data
            break;
        }     
    }

    FMatComparer::MaxMatsTable = UETable;
}

void FMatComparer::SwapMaterials() {
    // For each mesh in the Datasmith geometries
    for (auto mesh : FMatComparer::AssetMeshes)
    {
        if (mesh)
        {
            int numberOfMaterials = mesh->GetStaticMaterials().Num();
            for (int index = 0; index < numberOfMaterials; index++)
            {
                UMaterialInterface* meshMat = mesh->GetMaterial(index);
                if (meshMat)
                {
                    UEMatComparer* UEmaterialMatch = FMatComparer::GetUeMatMatch(meshMat, FMatComparer::UnrealMats); //get match from maxMats table

                    if (UEmaterialMatch) { //if match exists
                        // find real unreal material which name matches the UMaterialMatch in the table
                        TArray<UMaterialInterface*> unrealLibMats = FMatComparer::RealUnrealMats.FilterByPredicate([&](const UMaterialInterface* realLibMat) {
                            return realLibMat->GetFName() == UEmaterialMatch->UMaterialMatch;
                            });
                        
                        if (unrealLibMats.Num() > 0) {
                            UMaterialInterface* unrealLibMat = unrealLibMats.Pop();
                            //if match is found in unreal library, swap datasmith material for unrealLibMaterial
                            if (unrealLibMat) mesh->SetMaterial(index, unrealLibMat);
                        }
                    }
                    // Suggestions
                    //if (!UEmaterialMatch) {
                    //    MatComparer.GetUEMatSuggestions(realUEmat, MatComparer.UnrealMats);
                    //    //TODO MATCH UNREALMAT WITH REAL UNREAL MAT TO SET NEW MATERIAL

                    //} 
                }
            }
        }
    }
    
}

void FMatComparer::GetUEMatSuggestions(UMaterialInterface* maxmat, TArray<UEMatComparer*>& Unrealmats) {
    Unrealmats.Sort([&](UEMatComparer& uem1, UEMatComparer& uem2)
        {

            float m1match = FMatComparer::TextureCheck(*maxmat, uem1);
            float m2match = FMatComparer::TextureCheck(*maxmat, uem2);

            m1match += FMatComparer::FatherCheck(*maxmat, uem1);
            m2match += FMatComparer::FatherCheck(*maxmat, uem2);

            m1match += FMatComparer::ScalarParamsCheck(*maxmat, uem1, 0.25);
            m2match += FMatComparer::ScalarParamsCheck(*maxmat, uem2, 0.25);

            m1match += FMatComparer::VectorParamsCheck(*maxmat, uem1, 0.25);
            m2match += FMatComparer::VectorParamsCheck(*maxmat, uem2, 0.25);

            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, uem1.GetFName().ToString().Append(uem2.GetFName().ToString()));
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("values m1%f m2%f"), m1match, m2match));

            return m1match >= m2match;
        });

    for (auto um : Unrealmats) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, um->MaterialName.ToString());
    }
}

void FMatComparer::GenerateCSVwMaxMaterials(const FString SavePath, TArray<UMaterialInterface*> Assetmats) {
    
    FString CSV(TEXT(",MaterialName,UMaterialMatch,FatherName,TextureNames,ScalarParamValues,VectorParamValues, "));
    
    CSV.Append("\n");
    for (size_t it = 0; it < Assetmats.Num(); it++){
        CSV.Append(FMatComparer::MaxMatToFTableMat(Assetmats[it], it+1));
    }
    
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FString FilePath = FPaths::ConvertRelativePathToFull(SavePath) + TEXT("/UnrealTableMats.csv");
    FFileHelper::SaveStringToFile(CSV, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get());// , EFileWrite::FILEWRITE_AllowRead);
    UDataTable* UETable = LoadObject<UDataTable>(NULL, UTF8_TO_TCHAR("DataTable'/Game/Datasmith/MatComparer/MaxMats.MaxMats'"));

    if (!PlatformFile.FileExists(*FPaths::ConvertRelativePathToFull(*FilePath))) { //If file that was created doesn't exist
        const EAppReturnType::Type Choice = FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString(TEXT("Something went wrong... CSV couldn't be created.")));
    } 
    
    UETable->CreateTableFromCSVString(CSV);
    FMatComparer::MaxMatsTable = UETable;
    //FFileHelper::SaveStringToFile(SaveText, *(FPaths::GameDir() + OutFile), EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
    
    
}

FString FMatComparer::MaxMatToFTableMat(UMaterialInterface* maxmat, int it) {
    FString fathername = maxmat->GetBaseMaterial()->GetFName().ToString();
    FString materialname = maxmat->GetFName().ToString();
    FString scalarparams = "";
    FString vecparams = "";
    FString texparams = "";

    TArray< FMaterialParameterInfo > ParamsMax;
    TArray <FGuid> IdMax;
    maxmat->GetAllScalarParameterInfo(ParamsMax, IdMax);
    float pmaxvalue;

    for (auto p : ParamsMax) {

        FString pName, paramString;
        p.Name.ToString().Split(TEXT(" ("), &pName, NULL);
        maxmat->GetScalarParameterValue(p, pmaxvalue);

        paramString = p.Name.ToString() + "=" + FString::SanitizeFloat(pmaxvalue) + ";";
        //paramString = pName + "=" + FString::SanitizeFloat(pmaxvalue) + ";";
        scalarparams.Append(paramString);

    }

    maxmat->GetAllVectorParameterInfo(ParamsMax, IdMax);
    FLinearColor pmaxvecvalue;

    for (auto p : ParamsMax) {

        FString pName, paramString;
        p.Name.ToString().Split(TEXT(" ("), &pName, NULL);
        maxmat->GetVectorParameterValue(p, pmaxvecvalue);
        FString formattedvec = pmaxvecvalue.ToString().Replace(TEXT(","), TEXT("|")); //(r=0.0 | g=0.0 | b=0.0 | a=0.0) for csv parsing
        
        paramString = p.Name.ToString() + "=" + formattedvec + ";";
        //paramString = pName + "=" + formattedvec + ";";
        vecparams.Append(paramString);
    }
    
    UTexture* texvalue;
    maxmat->GetAllTextureParameterInfo(ParamsMax, IdMax);
    TArray<UTexture*> TexMax, alltex;
    maxmat->GetUsedTextures(TexMax, EMaterialQualityLevel::High, true, ERHIFeatureLevel::SM5, true);
    int iter = 0;

    for (auto usedText : TexMax) { //For each udsed texture

        //find parameter name
        FString pName, paramString;
        TArray<FMaterialParameterInfo> param = ParamsMax.FilterByPredicate([&](const FMaterialParameterInfo p) {        
     
            maxmat->GetTextureParameterValue(p, texvalue);
            return texvalue->GetFName() == usedText->GetFName();
        });

        //add used texture w/ parameter name
        if (param.Num() > 0) {
            param.Pop().Name.ToString().Split(TEXT(" ("), &pName, NULL);

            paramString = pName + "=" + usedText->GetFName().ToString() + ";";
            texparams.Append(paramString);
        }
    }

    //FTableMaterial* tablemat = new FTableMaterial(materialname, fathername, texparams, scalarparams, vecparams);
    return (FString::FromInt(it) + "," + materialname + "," + ""  + "," + fathername + "," + texparams + "," + scalarparams + "," + vecparams + "\n");
}

#undef LOCTEXT_NAMESPACE

/*UDataTable* UETable = (FMatComparer::GetDataTables(TablePath).FilterByPredicate([](UDataTable* asset) {
        return asset->GetFName().ToString().Contains(TEXT("MaxMats"));
        }).Pop());*/
        //UDataTable* UETable = LoadObject<UDataTable>(NULL, UTF8_TO_TCHAR("DataTable'/Game/Datasmith/MatComparer/MaxMats.MaxMats'"));
        //UETable->RowStruct = FTableMaterial::StaticStruct();
        //if (!UETable ||!UETable->GetRowStruct()->IsChildOf(FTableMaterial::StaticStruct())){

        //    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, UETable->GetRowStruct()->GetFName().ToString());
        //    return;

        //}
        //UETable->Modify();
        ////FTableMaterial* tablemat = new FTableMaterial(maxmat->GetFName().ToString(), "", "", "", "");
        //FName NewName = DataTableUtils::MakeValidName(TEXT("NewRow"));
        //const TArray<FName> ExisitngNames = UETable->GetRowNames();
        //while (ExisitngNames.Contains(NewName))
        //{
        //    NewName.SetNumber(NewName.GetNumber() + 1);
        //}
        //FDataTableEditorUtils::AddRow(UETable, NewName);

        //FString ContextString;
        //FTableMaterial* data = UETable->FindRow<FTableMaterial>(NewName, ContextString);