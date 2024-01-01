// Copyright 2024 Jesus Bracho All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RTSHUD.generated.h"

UCLASS()
class OPENRTSCAMERA_API ARTSHUD : public AHUD
{
	GENERATED_BODY()
public:
	// Function to be called to update the selection box
	void UpdateSelectionBox(const FVector2D& StartPoint, const FVector2D& EndPoint);
	void ClearSelectionBox();

protected:
	virtual void DrawHUD() override;

private:
	bool bIsSelecting;
	FVector2D SelectionStart;
	FVector2D SelectionEnd;
};

