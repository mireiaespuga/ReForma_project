#include "DBTab.h"
#include "ReForma_projectEditor/ReForma_projectEditor.h"

//#include "DBTabPanel.h"


#define LOCTEXT_NAMESPACE "DBTab"

class DBCommands : public TCommands<DBCommands>
{
public:

    DBCommands() : TCommands<DBCommands>
        (
        FName(TEXT("DBTabCommand")), // Context name for fast lookup
        FText::FromString("FormaDisseny tool"), // Context name for displaying
        NAME_None,   // No parent context
        FEditorStyle::GetStyleSetName() // Icon Style Set
        )
    {
    }


    virtual void RegisterCommands() override {
        UI_COMMAND(ReloadCommand, "Reload Database", "Reimport database data", EUserInterfaceActionType::Button, FInputGesture());
    }
    
    public:
    TSharedPtr<FUICommandInfo> ReloadCommand;
};



void DBTab::InstertIntoDB(FString tablename, int newname, FTableMaterial* tablemat) {
    if (tablename != "NONE") {
        FString insertQuery = "INSERT INTO " + tablename + " (RowName, MaterialName, UMaterialMatch, FatherName, TextureNames, ScalarParamValues, VectorParamValues)";
        insertQuery += "VALUES (";
        insertQuery += FString::FromInt(newname) + ",";
        insertQuery += "'" + tablemat->MaterialName + "'" + ",";
        insertQuery += "'" + tablemat->UMaterialMatch + "'" + ",";
        insertQuery += "'" + tablemat->FatherName + "'" + ",";
        insertQuery += "'" + tablemat->TextureNames + "'" + ",";
        insertQuery += "'" + tablemat->ScalarParamValues + "'" + ",";
        insertQuery += "'" + tablemat->VectorParamValues + "'" + ");";
        FReForma_projectEditor::Get().GetDB()->MySQLConnectorExecuteQuery(insertQuery, FReForma_projectEditor::Get().getConnection());
    }

}

int DBTab::GetLastDictEntry(FString tablename) {
    FString Query = "SELECT MAX(RowName) FROM " + tablename + ";";
    FMySQLConnectoreQueryResult resultQuery = FReForma_projectEditor::Get().GetDB()->MySQLConnectorGetData(Query, FReForma_projectEditor::Get().getConnection());
    return resultQuery.Success ? FCString::Atoi(*resultQuery.ResultRows[0].Fields[0].Value) : 0;
}

FString DBTab::CreateArtistTable() {
    UMySQLConnection* cs = FReForma_projectEditor::Get().GetDB()->MySQLInitConnection("localhost", "default_user", "Forma1235", "ReFormaDB");//"localhost", "root", "ReForma2021#!", "ReFormaDB");
    TArray<FMySQLConnectorTableField> Fields;

    bool bIsPk = true, bIsUnique = true, bIsNotNull = true;
    Fields.Push(FReForma_projectEditor::Get().GetDB()->MySQLConnectorINT("RowName", bIsPk, true));
    Fields.Push(FReForma_projectEditor::Get().GetDB()->MySQLConnectorVARCHAR("MaterialName", "50", !bIsPk, bIsUnique, bIsNotNull));
    Fields.Push(FReForma_projectEditor::Get().GetDB()->MySQLConnectorVARCHAR("UMaterialMatch", "50", !bIsPk, !bIsUnique, !bIsNotNull));
    Fields.Push(FReForma_projectEditor::Get().GetDB()->MySQLConnectorVARCHAR("FatherName", "50", !bIsPk, !bIsUnique, !bIsNotNull));
    Fields.Push(FReForma_projectEditor::Get().GetDB()->MySQLConnectorVARCHAR("TextureNames", "500", !bIsPk, !bIsUnique, !bIsNotNull));
    Fields.Push(FReForma_projectEditor::Get().GetDB()->MySQLConnectorVARCHAR("ScalarParamValues", "500", !bIsPk, !bIsUnique, !bIsNotNull));
    Fields.Push(FReForma_projectEditor::Get().GetDB()->MySQLConnectorVARCHAR("VectorParamValues", "500", !bIsPk, !bIsUnique, !bIsNotNull));

    FString user = FReForma_projectEditor::Get().GetUserID();
    if (user != "NONE") {
        FMySQLConnectorTable t = FReForma_projectEditor::Get().GetDB()->CreateTable(user, Fields, cs);
        return t.TableName;
    }
    else return "NONE";
}

void DBTab::loadArtistDB(UDataTable*& UETable) {

    TArray<FMySQLConnectorTableField> Fields;
    UMySQLConnection* cs = FReForma_projectEditor::Get().GetDB()->MySQLInitConnection("localhost", "default_user", "Forma1235", "ReFormaDB");//"localhost", "root", "ReForma2021#!", "ReFormaDB");
    if (cs && UETable) {

        FString user = FReForma_projectEditor::Get().GetUserID();
        /*FReForma_projectEditor::Get().setConnection(cs);*/
        if (user != "NONE") {
            DBTab::CreateArtistTable();     //If table does not exist create it
            FTableMaterial* tablemat = new FTableMaterial();
            FString Query = "SELECT * FROM " + user + ";";
            FMySQLConnectoreQueryResult resultQuery = FReForma_projectEditor::Get().GetDB()->MySQLConnectorGetData(Query, cs);
            FString rowname, context;
            TArray<FName> rowsToDelete;

            if (resultQuery.Success) {

                for (auto res : resultQuery.ResultRows) {
                    for (FMySQLConnectorKeyValuePair field : res.Fields) {
                        if (field.Key == "RowName") rowname = field.Value;
                        if (field.Key == "MaterialName") tablemat->MaterialName = field.Value;
                        if (field.Key == "UMaterialMatch") tablemat->UMaterialMatch = field.Value;
                        if (field.Key == "FatherName") tablemat->FatherName = field.Value;
                        if (field.Key == "TextureNames") tablemat->TextureNames = field.Value;
                        if (field.Key == "ScalarParamValues") tablemat->ScalarParamValues = field.Value;
                        if (field.Key == "VectorParamValues") tablemat->VectorParamValues = field.Value;
                        tablemat->isMasterDictEntry = false;
                    }

                    UETable->AddRow(FName(rowname), *tablemat);
                }
            }

            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FReForma_projectEditor::Get().GetUserID());
        }

    }
}

void DBTab::loadMasterDB(UDataTable*& UETable) {

    FTableMaterial* tablemat = new FTableMaterial();
    FString Query = "SELECT * FROM MainMatDictionary";
    FMySQLConnectoreQueryResult resultQuery = FReForma_projectEditor::Get().GetDB()->MySQLConnectorGetData(Query, FReForma_projectEditor::Get().getConnection());
    FString rowname, context;
    int lastDictEntry = DBTab::GetLastDictEntry("MainMatDictionary");
    TArray<FName> rowsToDelete;

    if (resultQuery.Success) {

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
        }
    }
}

void DBTab::ReloadDBCommand() { 
    if (FReForma_projectEditor::Get().CloseOpenEditors()) {
        FReForma_projectEditor::Get().InitializeDB();
    }
}

void DBTab::OnStartupModule()
{
    CommandList = MakeShareable(new FUICommandList);
    DBCommands::Register();
    DBTab::MapCommands();

    FReForma_projectEditor::Get().AddMenuExtension(FMenuExtensionDelegate::CreateRaw(this, &DBTab::MakeMenuEntry_), FName("DataBase"), CommandList);
}

void DBTab::MakeMenuEntry_(FMenuBuilder& menuBuilder)
{
   menuBuilder.AddMenuEntry(DBCommands::Get().ReloadCommand);
}


void DBTab::OnShutdownModule()
{
    DBCommands::Unregister();
}

void DBTab::MapCommands()
{
    const auto& Commands = DBCommands::Get();
    
    CommandList->MapAction(
        Commands.ReloadCommand,
        FExecuteAction::CreateSP(this, &DBTab::ReloadDBCommand), FCanExecuteAction());
}

#undef LOCTEXT_NAMESPACE
//TSharedRef<SDockTab> DBTab::SpawnTab(const FSpawnTabArgs& TabSpawnArgs)
//{
//    TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
//        .TabRole(ETabRole::NomadTab)
//        [
//            SNew(SDBTabPanel)
//            .Tool(SharedThis(this))
//        ];
//
//    return SpawnedTab;
//}

