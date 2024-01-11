#include "RTSHUD.h"
#include "RTSSelector.h"
#include "Engine/Canvas.h"

// Constructor implementation: Initializes default values.
ARTSHUD::ARTSHUD()
{
	SelectionBoxColor = FLinearColor::Green;
	SelectionBoxThickness = 1.0f;
	bIsDrawingSelectionBox = false;
	bIsPerformingSelection = false;
}

// Implementation of the DrawHUD function. It's called every frame to draw the HUD.
void ARTSHUD::DrawHUD()
{
	Super::DrawHUD(); // Call the base class implementation.

	// Draw the selection box if it's active.
	if (bIsDrawingSelectionBox)
	{
		DrawSelectionBox(SelectionStart, SelectionEnd);
	}

	// Perform selection actions if required.
	if (bIsPerformingSelection)
	{
		PerformSelection();
	}
}

// Starts the selection process, setting the initial point and activating the selection flag.
void ARTSHUD::BeginSelection(const FVector2D& StartPoint)
{
	SelectionStart = StartPoint;
	bIsDrawingSelectionBox = true;
}

// Updates the current endpoint of the selection box.
void ARTSHUD::UpdateSelection(const FVector2D& EndPoint)
{
	SelectionEnd = EndPoint;
}

// Ends the selection process and triggers the selection logic.
void ARTSHUD::EndSelection()
{
	bIsDrawingSelectionBox = false;
	bIsPerformingSelection = true;
}

// Default implementation of DrawSelectionBox. Draws a rectangle on the HUD.
void ARTSHUD::DrawSelectionBox_Implementation(const FVector2D& StartPoint, const FVector2D& EndPoint)
{
	if (Canvas)
	{
		// Calculate corners of the selection rectangle.
		const auto TopRight = FVector2D(SelectionEnd.X, SelectionStart.Y);
		const auto BottomLeft = FVector2D(SelectionStart.X, SelectionEnd.Y);

		// Draw lines to form the selection rectangle.
		Canvas->K2_DrawLine(SelectionStart, TopRight, SelectionBoxThickness, SelectionBoxColor);
		Canvas->K2_DrawLine(TopRight, SelectionEnd, SelectionBoxThickness, SelectionBoxColor);
		Canvas->K2_DrawLine(SelectionEnd, BottomLeft, SelectionBoxThickness, SelectionBoxColor);
		Canvas->K2_DrawLine(BottomLeft, SelectionStart, SelectionBoxThickness, SelectionBoxColor);
	}
}

// Default implementation of PerformSelection. Selects actors within the selection box.
void ARTSHUD::PerformSelection_Implementation()
{
	// Array to store actors that are within the selection rectangle.
	TArray<AActor*> SelectedActors;
	GetActorsInSelectionRectangle<AActor>(SelectionStart, SelectionEnd, SelectedActors, false, false);

	// Find the URTSSelector component and pass the selected actors to it.
	if (const auto PC = GetOwningPlayerController())
	{
		if (const auto SelectorComponent = PC->FindComponentByClass<URTSSelector>())
		{
			SelectorComponent->HandleSelectedActors(SelectedActors);
		}
	}

	bIsPerformingSelection = false;
}
