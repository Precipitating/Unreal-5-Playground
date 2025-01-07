// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerVisualizer.h"


void FPlayerVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	// get character 
	const AMainCharacter* mainCharacter = Cast<AMainCharacter>(Component);

	if (mainCharacter)
	{
		PDI->DrawLine(
			mainCharacter->GetActorLocation(),
			mainCharacter->GetActorLocation() + FVector(0,1000,0),
			FLinearColor::Red,
			SDPG_Foreground
			);
	}



}
