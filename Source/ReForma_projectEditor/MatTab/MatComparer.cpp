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

int FMatComparer::GetLastDictEntry() {
    FString Query = "SELECT MAX(RowName) FROM MainMatDictionary;";
    FMySQLConnectoreQueryResult resultQuery = FMatComparer::db->MySQLConnectorGetData(Query, FReForma_projectEditor::Get().getConnection());
    return resultQuery.Success ? FCString::Atoi(*resultQuery.ResultRows[0].Fields[0].Value) : 0;
}

void FMatComparer::initDB() {
    UMySQLConnection* cs = FMatComparer::db->MySQLInitConnection("localhost", "mespuga", "mespuga", "ReFormaDB");//"localhost", "root", "ReForma2021#!", "ReFormaDB");
    UDataTable* UETable = LoadObject<UDataTable>(NULL, UTF8_TO_TCHAR("DataTable'/Game/Datasmith/MatComparer/MaxMats.MaxMats'"));
    if (cs && UETable) {

        //FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString((TEXT("No materials were swapped."))));

        FReForma_projectEditor::Get().setConnection(cs);
       
        
        FTableMaterial* tablemat = new FTableMaterial();
        FTableMaterial* existingtablemat = new FTableMaterial();

        UETable->RowStruct = FTableMaterial::StaticStruct();

        FString Query = "SELECT * FROM MainMatDictionary";
        FMySQLConnectoreQueryResult resultQuery = FMatComparer::db->MySQLConnectorGetData(Query, FReForma_projectEditor::Get().getConnection());
        FString rowname, context;
        TArray<FTableMaterial*> matsToRelocate;
        int lastDictEntry = FMatComparer::GetLastDictEntry();
        TArray<FName> rowsToDelete;


        if (resultQuery.Success) {

            //store artists materials in aux array
            for (auto row : UETable->GetRowMap()) {
                FTableMaterial* data = (FTableMaterial*)(row.Value);
                if (!data->isMasterDictEntry) {
                    matsToRelocate.Push(data);
                }
            }

            UETable->EmptyTable();

            for (auto res : resultQuery.ResultRows) {
                for (FMySQLConnectorKeyValuePair field : res.Fields) {
                    if (field.Key == "RowName") rowname = field.Value;
                    if (field.Key == "MaterialName") tablemat->MaterialName = field.Value;
                    if (field.Key == "UMaterialMatch") tablemat->UMaterialMatch = field.Value;
                    if (field.Key == "FatherName") tablemat->FatherName = field.Value;
                    if (field.Key == "TextureNames") tablemat->TextureNames = field.Value;
                    if (field.Key == "ScalarParamValues") tablemat->ScalarParamValues = field.Value;
                    if (field.Key == "VectorParamValues") tablemat->VectorParamValues = field.Value;
                    tablemat->isMasterDictEntry = true;
                }

                UETable->AddRow(FName(rowname), *tablemat);
                ////check if row already exists
                //existingtablemat = UETable->FindRow<FTableMaterial>(FName(rowname), context);
                //if (existingtablemat) { //if row exists        
                //    GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, rowname + "   " + existingtablemat->MaterialName);

                //    if (existingtablemat != tablemat) { //we prioritize DB rowname --> relocate existing
                //        UETable->RemoveRow(FName(rowname));
                //        UETable->AddRow(FName(rowname), *tablemat);

                //        //matsToRelocate.Push(existingtablemat);
                //        int lastRowIndex = UETable->GetRowNames().Num() > 0 ? FCString::Atoi(*UETable->GetRowNames().Last().ToString()) : 0;
                //        int newname = lastRowIndex + 1 < lastDictEntry + 1 ? lastDictEntry++ : lastRowIndex++;
                //        UETable->AddRow(FName(FString::FromInt(newname)), *existingtablemat);
                //    }
                //}
                //else {
                //    UETable->AddRow(FName(rowname), *tablemat);
                //}
            }

            for (auto relocMat : matsToRelocate) {  //we move artits' mats to another row
                int lastRowIndex = FMatComparer::GetLastRowIndex(UETable);
                int newname = lastRowIndex < lastDictEntry ? lastDictEntry + 1 : lastRowIndex + 1;
                UETable->AddRow(FName(FString::FromInt(newname)), *relocMat);
            }
        }



        FMatComparer::DictionaryTable = UETable;
        //return resultQuery.Success;
    }
}

TArray<UEMatComparer*> FMatComparer::GetUEMaterials(const FString type) {

    TArray<UEMatComparer*> UEMats;
    if (type == "DICTIONARY") {
        UDataTable* UETable = LoadObject<UDataTable>(NULL, UTF8_TO_TCHAR("DataTable'/Game/Datasmith/MatComparer/MaxMats.MaxMats'"));
        FMatComparer::DictionaryTable = UETable;


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
    }
    else if (type == "SCENE") {
        for (auto asset : FMatComparer::AssetMats) {
            UEMatComparer* uemat = new UEMatComparer();
            uemat->MaterialName = asset->GetFName();
            if (FMatComparer::DictionaryMats.Num() > 0) {
                UEMatComparer* matched = FMatComparer::GetUeMatMatch(asset, FMatComparer::DictionaryMats);
                if (matched) uemat->UMaterialMatch = matched->UMaterialMatch;
            }
            UEMats.Push(uemat);
        }
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
        
        if (maxmat.GetScalarParameterValue(p, pmaxvalue)) {
            float pUevalue;
            if (exactMatch) {
                pUevalue = uemat.ScalarValueParams.Contains(p.Name.ToString()) ? uemat.ScalarValueParams[p.Name.ToString()] : -1.0f;
            }
            else {
                //TODO: if (9) is irrelevant remove it at the end of p.Name. i.e Fresnel_IOR (9) -> Fresnel_IOR
                FString pName;
                p.Name.ToString().Split(TEXT(" ("), &pName, NULL);
                TMap <FString, float> filteredparams = uemat.ScalarValueParams.FilterByPredicate([&](TPair <FString, float> sca) {
                    FString auxName;
                    sca.Key.Split(TEXT(" ("), &auxName, NULL);
                    return auxName == pName;
                    });
                TArray<FString> keys;
                filteredparams.GenerateKeyArray(keys);
                pUevalue = filteredparams.Num() > 0 ? filteredparams[keys[0]] : -1.0f;
            }
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
        
        if (maxmat.GetVectorParameterValue(p, pmaxvalue)) {
            FLinearColor pUevalue;
            if (exactMatch) {
                pUevalue = uemat.VectorValueParams.Contains(p.Name.ToString()) ? uemat.VectorValueParams[p.Name.ToString()] : FLinearColor(-1.0f, -1.0f, -1.0f, -1.0f);
            }
            else {
                //TODO: if (9) is irrelevant remove it at the end of p.Name. i.e Fresnel_IOR (9) -> Fresnel_IOR
                FString pName;
                p.Name.ToString().Split(TEXT(" ("), &pName, NULL);
                TMap <FString, FLinearColor> filteredparams = uemat.VectorValueParams.FilterByPredicate([&](TPair <FString, FLinearColor> vec) {
                    FString auxName;
                    vec.Key.Split(TEXT(" ("), &auxName, NULL);
                    return auxName == pName;
                });
                TArray<FString> keys;
                filteredparams.GenerateKeyArray(keys);
                pUevalue = filteredparams.Num() > 0 ? filteredparams[keys[0]] : FLinearColor(-1.0f, -1.0f, -1.0f, -1.0f);
            }
        
            matches = pmaxvalue.Equals(pUevalue) ? matches + matchWeight : matches; //have same scalar param value
        }
    }

    if (exactMatch) return matches == VecParamsMax.Num() * matchWeight || noVect ? 1.0f : 0.0f;
    else return matches;

}

void FMatComparer::SetUeMatMatch(UMaterialInterface* realuemat, UEMatComparer*& maxmatmatch) {
    
    //CHAGE ITTTT SINCE YOU WON'T FIND ROW
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

    FMatComparer::DictionaryTable = UETable;
}

void FMatComparer::SwapMaterials() {
    bool bSomethingChanged = false;
    int matSwaps = 0;
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
                    //UEMatComparer* UEmaterialMatch = FMatComparer::GetUeMatMatch(meshMat, FMatComparer::DictionaryMats); //get match from maxMats table
                    TArray<UEMatComparer*> UEmaterialMatchs = FMatComparer::SceneMats.FilterByPredicate([&](UEMatComparer* scenemat) {
                            return FMatComparer::MatNameCheck(*meshMat, *scenemat);
                        });

                    if (UEmaterialMatchs.Num() >0) { //if match exists

                        UEMatComparer* UEmaterialMatch = UEmaterialMatchs.Pop();
                        // find real unreal material which name matches the UMaterialMatch in the table
                        TArray<UMaterialInterface*> unrealLibMats = FMatComparer::RealUnrealMats.FilterByPredicate([&](const UMaterialInterface* realLibMat) {
                            return realLibMat->GetFName() == UEmaterialMatch->UMaterialMatch;
                            });
                        
                        if (unrealLibMats.Num() > 0) {
                            UMaterialInterface* unrealLibMat = unrealLibMats.Pop();
                            //if match is found in unreal library, swap datasmith material for unrealLibMaterial
                            if (unrealLibMat) {
                                mesh->SetMaterial(index, unrealLibMat);
                                matSwaps++;
                                bSomethingChanged = true;
                            }
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
    if(bSomethingChanged) FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString(FString::Printf(TEXT("Swap of %d material(s) was successful!"), matSwaps)));
    else FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString((TEXT("No materials were swapped."))));
}

TArray<UEMatComparer*> FMatComparer::GetUEMatSuggestions(UMaterialInterface* realuemat, TArray<UEMatComparer*> mats) {

    mats.Sort([&](UEMatComparer& uem1, UEMatComparer& uem2)
        {
            float m1match = FMatComparer::TextureCheck(*realuemat, uem1);
            float m2match = FMatComparer::TextureCheck(*realuemat, uem2);

            m1match += FMatComparer::FatherCheck(*realuemat, uem1);
            m2match += FMatComparer::FatherCheck(*realuemat, uem2);

            m1match += FMatComparer::ScalarParamsCheck(*realuemat, uem1, 0.25);
            m2match += FMatComparer::ScalarParamsCheck(*realuemat, uem2, 0.25);

            m1match += FMatComparer::VectorParamsCheck(*realuemat, uem1, 0.25);
            m2match += FMatComparer::VectorParamsCheck(*realuemat, uem2, 0.25);

            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, uem1.GetFName().ToString().Append(uem2.GetFName().ToString()));
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("values m1%f m2%f"), m1match, m2match));

            return m1match >= m2match;
        });
    
    return mats;
}

void FMatComparer::GenerateCSVwMaxMaterials(const FString SavePath, TArray<UMaterialInterface*> Assetmats) {
    
    FTableMaterial* t;
    FString CSV(TEXT(",MaterialName,UMaterialMatch,FatherName,TextureNames,ScalarParamValues,VectorParamValues, "));
    
    CSV.Append("\n");
    for (size_t it = 0; it < Assetmats.Num(); it++){
        CSV.Append(FMatComparer::MaxMatToFTableMat(Assetmats[it], it+1, t));
        if (it == 15) break; //TODO: remove
    }
    
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FString FilePath = FPaths::ConvertRelativePathToFull(SavePath) + TEXT("/UnrealTableMats.csv");
    FFileHelper::SaveStringToFile(CSV, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get());// , EFileWrite::FILEWRITE_AllowRead);
    UDataTable* UETable = LoadObject<UDataTable>(NULL, UTF8_TO_TCHAR("DataTable'/Game/Datasmith/MatComparer/MaxMats.MaxMats'"));

    if (!PlatformFile.FileExists(*FPaths::ConvertRelativePathToFull(*FilePath))) { //If file that was created doesn't exist
        const EAppReturnType::Type Choice = FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Cancel, FText::FromString(TEXT("Something went wrong... CSV couldn't be created.")));
    }  
    
    UETable->CreateTableFromCSVString(CSV);
    FMatComparer::DictionaryTable = UETable;
    //FFileHelper::SaveStringToFile(SaveText, *(FPaths::GameDir() + OutFile), EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
    
}

int FMatComparer::GetLastRowIndex(UDataTable* table) {
    TArray<FName> rownames = table->GetRowNames();
    rownames.Sort([](FName r1, FName r2) { return FCString::Atoi(*r1.ToString()) < FCString::Atoi(*r2.ToString()) /*r1.LexicalLess(r2)*/; });
    return table->GetRowNames().Num() > 0 ? FCString::Atoi(*rownames.Last().ToString()) : 0;
}

bool FMatComparer::AddMaterialToDict(UMaterialInterface* assetToImport) {
    UDataTable* UETable = LoadObject<UDataTable>(NULL, UTF8_TO_TCHAR("DataTable'/Game/Datasmith/MatComparer/MaxMats.MaxMats'"));
    
    int lastRowIndex = FMatComparer::GetLastRowIndex(UETable);

    FMatComparer::DictionaryMats = FMatComparer::GetUEMaterials("DICTIONARY");
    UEMatComparer* matched = FMatComparer::GetUeMatMatch(assetToImport, FMatComparer::DictionaryMats);
 
    if (!matched) { //there's no entry in table
        
        int lastDictEntry = FMatComparer::GetLastDictEntry();
        int newname = lastRowIndex < lastDictEntry? lastDictEntry + 1 : lastRowIndex + 1;
        FTableMaterial* tablemat = new FTableMaterial();
        FString outText = FMatComparer::MaxMatToFTableMat(assetToImport, newname, tablemat);

        UETable->RowStruct = FTableMaterial::StaticStruct();
        UETable->AddRow(FName(FString::FromInt(newname)), *tablemat);
        FMatComparer::DictionaryTable = UETable;

        //FString insertQuery = "INSERT INTO MainMatDictionary (RowName,MaterialName,UMaterialMatch,FatherName,TextureNames,ScalarParamValues,VectorParamValues)";
        //insertQuery += "VALUES (";
        //insertQuery += FString::FromInt(newname) + ",";
        //insertQuery += "'" + tablemat->MaterialName + "'" + ",";
        //insertQuery += "'" + tablemat->UMaterialMatch + "'" + ",";
        //insertQuery += "'" + tablemat->FatherName + "'" + ",";
        //insertQuery += "'" + tablemat->TextureNames + "'" + ",";
        //insertQuery += "'" + tablemat->ScalarParamValues + "'" + ",";
        //insertQuery += "'" + tablemat->VectorParamValues + "'" + ");";

    /*    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, insertQuery);
        UE_LOG(LogTemp, Warning, TEXT("%s"), *insertQuery);
        FMatComparer::db->MySQLConnectorExecuteQuery(insertQuery, FReForma_projectEditor::Get().getConnection());*/

        return true;
    }
    return false;
}

FString FMatComparer::MaxMatToFTableMat(UMaterialInterface* maxmat, int it, FTableMaterial*& intablemat) {
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
        else { //For textures such as emissive lights that are not registered as global textures => no param name but existing used texture
            paramString = "Other_Text=" + usedText->GetFName().ToString() + ";";
            texparams.Append(paramString);
        }
    }

    intablemat->MaterialName = materialname;
    intablemat->FatherName = fathername;
    intablemat->ScalarParamValues = scalarparams;
    intablemat->VectorParamValues = vecparams;
    intablemat->TextureNames = texparams;

    return (FString::FromInt(it) + "," + materialname + "," + ""  + "," + fathername + "," + texparams + "," + scalarparams + "," + vecparams + "\n");
}

#undef LOCTEXT_NAMESPACE


