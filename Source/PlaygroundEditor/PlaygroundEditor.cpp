#include "PlaygroundEditor.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"



IMPLEMENT_GAME_MODULE(PlaygroundEditorModule, PlaygroundEditor)

void PlaygroundEditorModule::StartupModule()
{
	// check if editor is vlaid

	if (GUnrealEd)
	{
		// register the visualizer for player
		TSharedPtr<FPlayerVisualizer> visualizer = MakeShareable(new FPlayerVisualizer);
		if (visualizer.IsValid())
		{
			GUnrealEd->RegisterComponentVisualizer(AMainCharacter::StaticClass()->GetFName(), visualizer);
			visualizer->OnRegister();
		}
		
	}
}

void PlaygroundEditorModule::ShutdownModule()
{
	GUnrealEd->UnregisterComponentVisualizer(AMainCharacter::StaticClass()->GetFName());
}
