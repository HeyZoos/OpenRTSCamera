// Copyright 2024 Jesus Bracho All Rights Reserved.


#include "RTSSelector.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "RTSSelectable.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
URTSSelector::URTSSelector(): bIsSelecting(false), PlayerController(nullptr), HUD(nullptr)
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

void URTSSelector::HandleSelectedActors(const TArray<AActor*>& NewSelectedActors)
{
	for (const auto Actor : NewSelectedActors)
	{
		if (URTSSelectable* SelectableComponent = Actor->FindComponentByClass<URTSSelectable>())
		{
			this->SelectedActors.Add(SelectableComponent);
			SelectableComponent->OnSelected();
		}
	}

	// Handle the selected actors as needed
	UE_LOG(LogTemp, Log, TEXT("Number of Selected Actors: %d"), SelectedActors.Num());
}

void URTSSelector::ClearSelectedActors()
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
	bIsSelecting = true;

	// Get the starting point of the selection box
	if (PlayerController)
	{
		FVector2D MousePosition;
		PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
		SelectionStart = MousePosition;
		UE_LOG(LogTemp, Log, TEXT("OnSelectionStart - Mouse Position: (X: %f, Y: %f)"),
		       MousePosition.X, MousePosition.Y);
	}
}

void URTSSelector::OnUpdateSelection(const FInputActionValue& Value)
{
	bIsSelecting = true;

	// Get the starting point of the selection box
	if (PlayerController)
	{
		FVector2D MousePosition;
		PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
		SelectionEnd = MousePosition;
		HUD->UpdateSelectionBox(SelectionStart, SelectionEnd);
	}
}

void URTSSelector::OnSelectionEnd(const FInputActionValue& Value)
{
	if (PlayerController)
	{
		if (ARTSHUD* RTS_HUD = Cast<ARTSHUD>(HUD))
		{
			// Call PerformSelection on the HUD to execute selection logic
			RTS_HUD->PerformSelection();
		}
	}
}
