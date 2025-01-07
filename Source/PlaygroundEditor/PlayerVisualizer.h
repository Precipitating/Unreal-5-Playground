// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComponentVisualizer.h"
#include "MainCharacter.h"
/**
 * 
 */
class PLAYGROUNDEDITOR_API FPlayerVisualizer: public FComponentVisualizer
{
public:
	void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
};
