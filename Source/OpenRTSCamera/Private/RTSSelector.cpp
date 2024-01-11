// Copyright 2024 Jesus Bracho All Rights Reserved.

#include "RTSSelector.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "RTSSelectable.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
URTSSelector::URTSSelector(): PlayerController(nullptr), HUD(nullptr), bIsSelecting(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Add defaults for input actions
	static ConstructorHelpers::FObjectFinder<UInputAction>
		BeginSelectionActionFinder(TEXT("/OpenRTSCamera/Inputs/BeginSelection"));
	static ConstructorHelpers::FObjectFinder<UInputMappingContext>
		InputMappingContextFinder(TEXT("/OpenRTSCamera/Inputs/OpenRTSCameraInputs"));
	this->BeginSelection = BeginSelectionActionFinder.Object;
	this->InputMappingContext = InputMappingContextFinder.Object;
}


// Called when the game starts
void URTSSelector::BeginPlay()
{
	Super::BeginPlay();

	const auto NetMode = this->GetNetMode();
	if (NetMode != NM_DedicatedServer)
	{
		this->CollectComponentDependencyReferences();
		this->BindInputMappingContext();
		this->BindInputActions();
		OnActorsSelected.AddDynamic(this, &URTSSelector::HandleSelectedActors);
	}
}

void URTSSelector::HandleSelectedActors_Implementation(const TArray<AActor*>& NewSelectedActors)
{
	// Convert NewSelectedActors to a set for efficient lookup
	TSet<AActor*> NewSelectedActorSet;
	for (const auto& Actor : NewSelectedActors)
	{
		NewSelectedActorSet.Add(Actor);
	}

	// Iterate over currently selected actors
	for (const auto& Selected : SelectedActors)
	{
		// Check if the actor is not in the new selection
		if (!NewSelectedActorSet.Contains(Selected->GetOwner()))
		{
			// Call OnDeselected for actors that are no longer selected
			Selected->OnDeselected();
		}
	}

	// Clear the current selection
	ClearSelectedActors();
    
	// Add new selected actors and call OnSelected
	for (const auto& Actor : NewSelectedActors)
	{
		if (URTSSelectable* SelectableComponent = Actor->FindComponentByClass<URTSSelectable>())
		{
			this->SelectedActors.Add(SelectableComponent);
			SelectableComponent->OnSelected();
		}
	}
}

void URTSSelector::ClearSelectedActors_Implementation()
{
	this->SelectedActors.Empty();
}

// Called every frame
void URTSSelector::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URTSSelector::CollectComponentDependencyReferences()
{
	if (const auto PlayerControllerRef = UGameplayStatics::GetPlayerController(this->GetWorld(), 0))
	{
		this->PlayerController = PlayerControllerRef;
		this->HUD = Cast<ARTSHUD>(PlayerControllerRef->GetHUD());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("USelector is not attached to a PlayerController."));
	}
}

void URTSSelector::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (const auto InputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		InputComponent->BindAction(this->BeginSelection, ETriggerEvent::Started, this, &URTSSelector::OnSelectionStart);
		InputComponent->BindAction(this->BeginSelection, ETriggerEvent::Completed, this, &URTSSelector::OnSelectionEnd);
	}
}

void URTSSelector::BindInputActions()
{
	if (const auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(this->PlayerController->InputComponent))
	{
		EnhancedInputComponent->BindAction(
			this->BeginSelection,
			ETriggerEvent::Started,
			this,
			&URTSSelector::OnSelectionStart
		);

		EnhancedInputComponent->BindAction(
			this->BeginSelection,
			ETriggerEvent::Triggered,
			this,
			&URTSSelector::OnUpdateSelection
		);

		EnhancedInputComponent->BindAction(
			this->BeginSelection,
			ETriggerEvent::Completed,
			this,
			&URTSSelector::OnSelectionEnd
		);
	}
}

void URTSSelector::BindInputMappingContext()
{
	if (PlayerController && PlayerController->GetLocalPlayer())
	{
		if (const auto Input = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			PlayerController->bShowMouseCursor = true;

			// Check if the context is already bound to prevent double binding
			if (!Input->HasMappingContext(this->InputMappingContext))
			{
				Input->ClearAllMappings();
				Input->AddMappingContext(this->InputMappingContext, 0);
			}
		}
	}
}

void URTSSelector::OnSelectionStart(const FInputActionValue& Value)
{
	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	HUD->BeginSelection(MousePosition);
}

void URTSSelector::OnUpdateSelection(const FInputActionValue& Value)
{
	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	SelectionEnd = MousePosition;
	HUD->UpdateSelection(SelectionEnd);
}

void URTSSelector::OnSelectionEnd(const FInputActionValue& Value)
{
	// Call PerformSelection on the HUD to execute selection logic
	HUD->EndSelection();
}
