#include "ReForma_projectEditor.h"
#include "IReFormaModuleInterface.h"
#include "MatTab/MatTab.h"


IMPLEMENT_GAME_MODULE(FReForma_projectEditor, ReForma_projectEditor)
TSharedRef<FWorkspaceItem> FReForma_projectEditor::MenuRoot = FWorkspaceItem::NewGroup(FText::FromString("Menu Root"));


void FReForma_projectEditor::AddModuleListeners()
{
    // add tools later
    ModuleListeners.Add(MakeShareable(new MatTab));
}

void FReForma_projectEditor::StartupModule()
{
    if (!IsRunningCommandlet())
    {
        FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
        LevelEditorMenuExtensibilityManager = LevelEditorModule.GetMenuExtensibilityManager();
        MenuExtender = MakeShareable(new FExtender);
        MenuExtender->AddMenuBarExtension("Window", EExtensionHook::After, NULL, FMenuBarExtensionDelegate::CreateRaw(this, &FReForma_projectEditor::MakePulldownMenu));
        LevelEditorMenuExtensibilityManager->AddExtender(MenuExtender);
    }

    IReFormaModuleInterface::StartupModule();
}

void FReForma_projectEditor::ShutdownModule()
{
    IReFormaModuleInterface::ShutdownModule();
}

void FReForma_projectEditor::AddMenuExtension(const FMenuExtensionDelegate& extensionDelegate, FName extensionHook, const TSharedPtr<FUICommandList>& CommandList, EExtensionHook::Position position)
{
    MenuExtender->AddMenuExtension(extensionHook, position, CommandList, extensionDelegate);
}

void FReForma_projectEditor::MakePulldownMenu(FMenuBarBuilder& menuBuilder)
{
    menuBuilder.AddPullDownMenu(
        FText::FromString("FormaDisseny"),
        FText::FromString("FormaDisseny related tools"),
        FNewMenuDelegate::CreateRaw(this, &FReForma_projectEditor::FillPulldownMenu),
        "FormaDisseny",
        FName(TEXT("FormaDissenyMenu"))
    );
}

void FReForma_projectEditor::FillPulldownMenu(FMenuBuilder& menuBuilder)
{
    // just a frame for tools to fill in
    menuBuilder.BeginSection("FormaSection", FText::FromString("Materials"));
    menuBuilder.AddMenuSeparator(FName("Materials"));
    menuBuilder.EndSection();

    //menuBuilder.BeginSection("ExampleSection", FText::FromString("Section 2"));
    //menuBuilder.AddMenuSeparator(FName("Section_2"));
    //menuBuilder.EndSection();
}