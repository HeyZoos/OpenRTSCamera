// Copyright 2024 Jesus Bracho All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "RTSHUD.h"
#include "RTSSelectable.h"
#include "Components/ActorComponent.h"
#include "RTSSelector.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OPENRTSCAMERA_API URTSSelector : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URTSSelector();
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorsSelected, const TArray<AActor*>&, SelectedActors);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Inputs")
	UInputMappingContext* InputMappingContext;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Inputs")
	UInputAction* BeginSelection;
	FOnActorsSelected OnActorsSelected;
	UPROPERTY()
	TArray<URTSSelectable*> SelectedActors;
	UFUNCTION()
	void HandleSelectedActors(const TArray<AActor*>& NewSelectedActors);
	void ClearSelectedActors();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);
	void BindInputActions();
	void BindInputMappingContext();

	// Input functions
	void OnSelectionStart(const FInputActionValue& Value);
	void OnUpdateSelection(const FInputActionValue& Value);
	void OnSelectionEnd(const FInputActionValue& Value);

private:
	void CollectComponentDependencyReferences();

	FVector2D SelectionStart;
	FVector2D SelectionEnd;
	bool bIsSelecting;
	UPROPERTY()
	APlayerController* PlayerController;
	UPROPERTY()
	ARTSHUD* HUD;
};
