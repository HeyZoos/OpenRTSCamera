// Copyright 2024 Jesus Bracho All Rights Reserved.


#include "RTSSelector.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/HUD.h"
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
	}
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
		this->HUD = PlayerControllerRef->GetHUD();
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

void URTSSelector::OnSelectionEnd(const FInputActionValue& Value)
{
	bIsSelecting = false;

	// Get the ending point of the selection box and perform selection
	if (PlayerController)
	{
		FVector2D MousePosition;
		PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
		SelectionEnd = MousePosition;
		UE_LOG(LogTemp, Log, TEXT("OnSelectionEnd - Mouse Position: (X: %f, Y: %f)"),
		       MousePosition.X, MousePosition.Y);

		SelectUnitsInBox(); // Function to select units within the box
	}
}


void URTSSelector::SelectUnitsInBox()
{
	if (HUD)
	{
		TArray<AActor*> SelectedActors;
		HUD->GetActorsInSelectionRectangle<AActor>(SelectionStart, SelectionEnd, SelectedActors, false, false);
		// Log the count of selected actors
		const int32 NumSelectedActors = SelectedActors.Num();
		UE_LOG(LogTemp, Log, TEXT("Number of Selected Actors: %d"), NumSelectedActors);

		// Log the start and end points of the selection
		UE_LOG(LogTemp, Log, TEXT("Selection Start: (X: %f, Y: %f), Selection End: (X: %f, Y: %f)"),
		       SelectionStart.X, SelectionStart.Y, SelectionEnd.X, SelectionEnd.Y);
	}
}
