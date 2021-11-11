#include "MatTab.h"
#include "ReForma_projectEditor/ReForma_projectEditor.h"
#include "MatTabPanel.h"


void MatTab::OnStartupModule()
{
    FCustomTabToolBase::OnStartupModule();
    FReForma_projectEditor::Get().AddMenuExtension(FMenuExtensionDelegate::CreateRaw(this, &MatTab::MakeMenuEntry), FName("Materials"));
}

void MatTab::OnShutdownModule()
{
    FCustomTabToolBase::OnShutdownModule();
}

void MatTab::Initialize()
{
    TabName = "MatTab";
    TabDisplayName = FText::FromString("Scene materials editor");
    ToolTipText = FText::FromString("Scene materials editor");
}

TSharedRef<SDockTab> MatTab::SpawnTab(const FSpawnTabArgs& TabSpawnArgs)
{
    TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SMatTabPanel)
            .Tool(SharedThis(this))
        ];

    return SpawnedTab;
}