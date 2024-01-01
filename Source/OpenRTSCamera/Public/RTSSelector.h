// Copyright 2024 Jesus Bracho All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Components/ActorComponent.h"
#include "RTSSelector.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENRTSCAMERA_API URTSSelector : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URTSSelector();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Inputs")
	UInputMappingContext* InputMappingContext;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Inputs")
	UInputAction* BeginSelection;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);
	void BindInputActions();
	void BindInputMappingContext();

	// Input functions
	void OnSelectionStart(const FInputActionValue& Value);
	void OnSelectionEnd(const FInputActionValue& Value);

	// Unit selection
	void SelectUnitsInBox();

	// Selection box drawing
	void DrawSelectionBox();

private:
	void CollectComponentDependencyReferences();
	
	FVector2D SelectionStart;
	FVector2D SelectionEnd;
	bool bIsSelecting;
	UPROPERTY()
	APlayerController* PlayerController;
	UPROPERTY()
	AHUD* HUD;
};
