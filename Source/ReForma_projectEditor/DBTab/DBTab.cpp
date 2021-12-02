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


void DBTab::ReloadDBCommand() { 
    if (FReForma_projectEditor::Get().CloseOpenTab()) {
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

