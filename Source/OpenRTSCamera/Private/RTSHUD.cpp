// Copyright 2024 Jesus Bracho All Rights Reserved.


#include "RTSHUD.h"

#include "RTSSelector.h"
#include "Engine/Canvas.h"


void ARTSHUD::DrawHUD()
{
	Super::DrawHUD();

	if (bIsSelecting)
	{
		// Ensure the Canvas is valid
		if (Canvas)
		{
			// Set the color and thickness of the lines
			FLinearColor LineColor = FLinearColor::Green; // You can change the color
			float LineThickness = 1.0f; // You can change the thickness

			// Calculate the corners of the rectangle
			FVector2D TopRight = FVector2D(SelectionEnd.X, SelectionStart.Y);
			FVector2D BottomLeft = FVector2D(SelectionStart.X, SelectionEnd.Y);

			// Draw the four sides of the rectangle
			Canvas->K2_DrawLine(SelectionStart, TopRight, LineThickness, LineColor);
			Canvas->K2_DrawLine(TopRight, SelectionEnd, LineThickness, LineColor);
			Canvas->K2_DrawLine(SelectionEnd, BottomLeft, LineThickness, LineColor);
			Canvas->K2_DrawLine(BottomLeft, SelectionStart, LineThickness, LineColor);
		}
	}

	// Perform the selection if the flag is set
	if (bPerformSelection)
	{
		TArray<AActor*> SelectedActors;
		GetActorsInSelectionRectangle<AActor>(SelectionStart, SelectionEnd, SelectedActors, false, false);
		bPerformSelection = false; // Reset the flag

		// Find the URTSSelector component and pass the selected actors to it
		if (const auto PC = GetOwningPlayerController())
		{
			if (const auto SelectorComponent = PC->FindComponentByClass<URTSSelector>())
			{
				SelectorComponent->HandleSelectedActors(SelectedActors);
			}
		}
	}
}

void ARTSHUD::UpdateSelectionBox(const FVector2D& StartPoint, const FVector2D& EndPoint)
{
	SelectionStart = StartPoint;
	SelectionEnd = EndPoint;
	bIsSelecting = true;
}

void ARTSHUD::ClearSelectionBox()
{
	bIsSelecting = false;
}

void ARTSHUD::PerformSelection()
{
	if (bIsSelecting)
	{
		bIsSelecting = false; // Reset the flag
		bPerformSelection = true;
	}
}
