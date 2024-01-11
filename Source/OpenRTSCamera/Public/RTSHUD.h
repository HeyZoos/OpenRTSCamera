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
	ARTSHUD();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Selection Box")
	FLinearColor SelectionBoxColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Selection Box")
	float SelectionBoxThickness;

	UFUNCTION(BlueprintCallable, Category = "Selection Box")
	void BeginSelection(const FVector2D& StartPoint);

	UFUNCTION(BlueprintCallable, Category = "Selection Box")
	void UpdateSelection(const FVector2D& EndPoint);

	UFUNCTION(BlueprintCallable, Category = "Selection Box")
	void EndSelection();

	UFUNCTION(BlueprintNativeEvent, Category = "Selection Box")
	void DrawSelectionBox(const FVector2D& StartPoint, const FVector2D& EndPoint);

	UFUNCTION(BlueprintNativeEvent, Category = "Selection Box")
	void PerformSelection();

protected:
	virtual void DrawHUD() override;

private:
	bool bIsDrawingSelectionBox;
	bool bIsPerformingSelection;
	FVector2D SelectionStart;
	FVector2D SelectionEnd;
};
