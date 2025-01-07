#pragma once
#include "Engine.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "UnrealEd.h"
#include "PlayerVisualizer.h"

DECLARE_LOG_CATEGORY_EXTERN(PlaygroundEditor, All, All)

class PlaygroundEditorModule : public IModuleInterface
{
public:
	void StartupModule() override;
	void ShutdownModule() override;

};