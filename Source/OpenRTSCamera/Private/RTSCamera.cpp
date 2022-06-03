// Copyright 2022 Jesus Bracho All Rights Reserved.

#include "RTSCamera.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "VectorTypes.h"

URTSCamera::URTSCamera()
{
	PrimaryComponentTick.bCanEverTick = true;
	this->CameraBlockingVolumeTag = FName("OpenRTSCamera#CameraBounds");
	this->EdgeScrollSpeed = 50;
	this->DistanceFromEdgeThreshold = 0.1f;
	this->EnableCameraLag = true;
	this->EnableCameraRotationLag = true;
	this->EnableDynamicCameraHeight = true;
	this->EnableEdgeScrolling = true;
	this->FindGroundTraceLength = 100000;
	this->MaximumZoomLength = 5000;
	this->MinimumZoomLength = 500;
	this->MoveSpeed = 50;
	this->RotateSpeed = 45;
	this->StartingYAngle = -45.0f;
	this->StartingZAngle = 0;
	this->ZoomCatchupSpeed = 4;
	this->ZoomSpeed = -200;

	static ConstructorHelpers::FObjectFinder<UInputAction>
		MoveCameraXAxisFinder(TEXT("/OpenRTSCamera/Inputs/MoveCameraXAxis"));
	static ConstructorHelpers::FObjectFinder<UInputAction>
		MoveCameraYAxisFinder(TEXT("/OpenRTSCamera/Inputs/MoveCameraYAxis"));
	static ConstructorHelpers::FObjectFinder<UInputAction>
		RotateCameraAxisFinder(TEXT("/OpenRTSCamera/Inputs/RotateCameraAxis"));
	static ConstructorHelpers::FObjectFinder<UInputAction>
		TurnCameraLeftFinder(TEXT("/OpenRTSCamera/Inputs/TurnCameraLeft"));
	static ConstructorHelpers::FObjectFinder<UInputAction>
		TurnCameraRightFinder(TEXT("/OpenRTSCamera/Inputs/TurnCameraRight"));
	static ConstructorHelpers::FObjectFinder<UInputAction>
		ZoomCameraFinder(TEXT("/OpenRTSCamera/Inputs/ZoomCamera"));
	static ConstructorHelpers::FObjectFinder<UInputMappingContext>
		InputMappingContextFinder(TEXT("/OpenRTSCamera/Inputs/OpenRTSCameraInputs"));

	this->MoveCameraXAxis = MoveCameraXAxisFinder.Object;
	this->MoveCameraYAxis = MoveCameraYAxisFinder.Object;
	this->RotateCameraAxis = RotateCameraAxisFinder.Object;
	this->TurnCameraLeft = TurnCameraLeftFinder.Object;
	this->TurnCameraRight = TurnCameraRightFinder.Object;
	this->ZoomCamera = ZoomCameraFinder.Object;
	this->InputMappingContext = InputMappingContextFinder.Object;
}

void URTSCamera::BeginPlay()
{
	Super::BeginPlay();
	this->CollectComponentDependencyReferences();
	this->ConfigureSpringArm();
	this->TryToFindBoundaryVolumeReference();
	this->ConditionallyEnableEdgeScrolling();
	this->CheckForEnhancedInputComponent();
	this->BindInputMappingContext();
	this->BindInputActions();
}

void URTSCamera::TickComponent(
	const float DeltaTime,
	const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	this->DeltaSeconds = DeltaTime;
	this->ConditionallyPerformEdgeScrolling();
	this->ConditionallyKeepCameraAtDesiredZoomAboveGround();
	this->SmoothTargetArmLengthToDesiredZoom();
	this->FollowTargetIfSet();
	this->ConditionallyApplyCameraBounds();
}

void URTSCamera::FollowTarget(AActor* Target)
{
	this->CameraFollowTarget = Target;
}

void URTSCamera::UnFollowTarget()
{
	this->CameraFollowTarget = nullptr;
}

void URTSCamera::OnZoomCamera(const FInputActionValue& Value)
{
	this->DesiredZoomLength = FMath::Clamp(
		this->DesiredZoomLength + Value.Get<float>() * this->ZoomSpeed,
		this->MinimumZoomLength,
		this->MaximumZoomLength
	);
}

void URTSCamera::OnRotateCamera(const FInputActionValue& Value)
{
	const auto WorldRotation = this->Root->GetComponentRotation();
	this->Root->SetWorldRotation(
		FRotator::MakeFromEuler(
			FVector(
				WorldRotation.Euler().X,
				WorldRotation.Euler().Y,
				WorldRotation.Euler().Z + Value.Get<float>()
			)
		)
	);
}

void URTSCamera::OnTurnCameraLeft(const FInputActionValue&)
{
	const auto WorldRotation = this->Root->GetRelativeRotation();
	this->Root->SetRelativeRotation(
		FRotator::MakeFromEuler(
			FVector(
				WorldRotation.Euler().X,
				WorldRotation.Euler().Y,
				WorldRotation.Euler().Z - this->RotateSpeed
			)
		)
	);
}

void URTSCamera::OnTurnCameraRight(const FInputActionValue&)
{
	const auto WorldRotation = this->Root->GetRelativeRotation();
	this->Root->SetRelativeRotation(
		FRotator::MakeFromEuler(
			FVector(
				WorldRotation.Euler().X,
				WorldRotation.Euler().Y,
				WorldRotation.Euler().Z + this->RotateSpeed
			)
		)
	);
}

void URTSCamera::OnMoveCameraYAxis(const FInputActionValue& Value)
{
	this->MoveSpringArmWithMoveSpeed(
		this->SpringArm->GetForwardVector().X,
		this->SpringArm->GetForwardVector().Y,
		Value.Get<float>()
	);
}

void URTSCamera::OnMoveCameraXAxis(const FInputActionValue& Value)
{
	this->MoveSpringArmWithMoveSpeed(
		this->SpringArm->GetRightVector().X,
		this->SpringArm->GetRightVector().Y,
		Value.Get<float>()
	);
}

void URTSCamera::MoveSpringArmWithMoveSpeed(const float X, const float Y, const float Scale) const
{
	auto Movement = FVector2D(X, Y);
	Movement.Normalize();
	Movement *= this->MoveSpeed * Scale;
	this->Root->SetWorldLocation(
		this->Root->GetComponentLocation() + FVector(Movement.X, Movement.Y, 0.0f)
	);
}

void URTSCamera::CollectComponentDependencyReferences()
{
	this->Owner = this->GetOwner();
	this->Root = this->Owner->GetRootComponent();
	this->Camera = Cast<UCameraComponent>(this->Owner->GetComponentByClass(UCameraComponent::StaticClass()));
	this->SpringArm = Cast<USpringArmComponent>(this->Owner->GetComponentByClass(USpringArmComponent::StaticClass()));
	this->PlayerController = UGameplayStatics::GetPlayerController(this->GetWorld(), 0);
}

void URTSCamera::ConfigureSpringArm()
{
	this->DesiredZoomLength = this->MaximumZoomLength;
	this->SpringArm->TargetArmLength = this->DesiredZoomLength;
	this->SpringArm->bDoCollisionTest = false;
	this->SpringArm->bEnableCameraLag = this->EnableCameraLag;
	this->SpringArm->bEnableCameraRotationLag = this->EnableCameraRotationLag;
	this->SpringArm->SetRelativeRotation(
		FRotator::MakeFromEuler(
			FVector(
				0.0,
				this->StartingYAngle,
				this->StartingZAngle
			)
		)
	);
}

void URTSCamera::TryToFindBoundaryVolumeReference()
{
	TArray<AActor*> BlockingVolumes;
	UGameplayStatics::GetAllActorsOfClassWithTag(
		this->GetWorld(),
		ACameraBoundsVolume::StaticClass(),
		this->CameraBlockingVolumeTag,
		BlockingVolumes
	);

	if (BlockingVolumes.Num() > 0)
	{
		this->BoundaryVolume = Cast<ACameraBoundsVolume>(BlockingVolumes[0]);
	}
}

void URTSCamera::ConditionallyEnableEdgeScrolling() const
{
	if (this->EnableEdgeScrolling)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		this->PlayerController->SetInputMode(InputMode);
	}
}

void URTSCamera::CheckForEnhancedInputComponent() const
{
	if (Cast<UEnhancedInputComponent>(this->PlayerController->InputComponent) == nullptr)
	{
		UKismetSystemLibrary::PrintString(
			this->GetWorld(),
			TEXT("Set Edit > Project Settings > Input > Default Classes to Enhanced Input Classes"), true, true,
			FLinearColor::Red,
			100
		);

		UKismetSystemLibrary::PrintString(
			this->GetWorld(),
			TEXT("Keyboard inputs will probably not function."), true, true,
			FLinearColor::Red,
			100
		);

		UKismetSystemLibrary::PrintString(
			this->GetWorld(),
			TEXT("Error: Enhanced input component not found."), true, true,
			FLinearColor::Red,
			100
		);
	}
}

void URTSCamera::BindInputMappingContext() const
{
	this->PlayerController->bShowMouseCursor = true;
	const auto Subsystem = this
	                       ->PlayerController
	                       ->GetLocalPlayer()
	                       ->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(this->InputMappingContext, 0);
}

void URTSCamera::BindInputActions()
{
	if (const auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(this->PlayerController->InputComponent))
	{
		EnhancedInputComponent->BindAction(
			this->ZoomCamera,
			ETriggerEvent::Triggered,
			this,
			&URTSCamera::OnZoomCamera
		);

		EnhancedInputComponent->BindAction(
			this->RotateCameraAxis,
			ETriggerEvent::Triggered,
			this,
			&URTSCamera::OnRotateCamera
		);

		EnhancedInputComponent->BindAction(
			this->TurnCameraLeft,
			ETriggerEvent::Triggered,
			this,
			&URTSCamera::OnTurnCameraLeft
		);

		EnhancedInputComponent->BindAction(
			this->TurnCameraRight,
			ETriggerEvent::Triggered,
			this,
			&URTSCamera::OnTurnCameraRight
		);

		EnhancedInputComponent->BindAction(
			this->MoveCameraXAxis,
			ETriggerEvent::Triggered,
			this,
			&URTSCamera::OnMoveCameraXAxis
		);

		EnhancedInputComponent->BindAction(
			this->MoveCameraYAxis,
			ETriggerEvent::Triggered,
			this,
			&URTSCamera::OnMoveCameraYAxis
		);
	}
}

void URTSCamera::ConditionallyPerformEdgeScrolling() const
{
	if (this->EnableEdgeScrolling)
	{
		this->EdgeScrollLeft();
		this->EdgeScrollRight();
		this->EdgeScrollUp();
		this->EdgeScrollDown();
	}
}

void URTSCamera::EdgeScrollLeft() const
{
	const auto MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this->GetWorld());
	const auto ViewportSize = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this->GetWorld()).GetLocalSize();
	const auto NormalizedMousePosition = 1 - UKismetMathLibrary::NormalizeToRange(
		MousePosition.X,
		0.0f,
		ViewportSize.X * 0.05f
	);

	const auto Movement = UKismetMathLibrary::FClamp(NormalizedMousePosition, 0.0, 1.0);

	this->Root->AddRelativeLocation(
		-1 * this->Root->GetRightVector() * Movement * this->EdgeScrollSpeed
	);
}

void URTSCamera::EdgeScrollRight() const
{
	const auto MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this->GetWorld());
	const auto ViewportSize = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this->GetWorld()).GetLocalSize();
	const auto NormalizedMousePosition = UKismetMathLibrary::NormalizeToRange(
		MousePosition.X,
		ViewportSize.X * 0.95f,
		ViewportSize.X
	);

	const auto Movement = UKismetMathLibrary::FClamp(NormalizedMousePosition, 0.0, 1.0);
	this->Root->AddRelativeLocation(
		this->Root->GetRightVector() * Movement * this->EdgeScrollSpeed
	);
}

void URTSCamera::EdgeScrollUp() const
{
	const auto MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this->GetWorld());
	const auto ViewportSize = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this->GetWorld()).GetLocalSize();
	const auto NormalizedMousePosition = UKismetMathLibrary::NormalizeToRange(
		MousePosition.Y,
		0.0f,
		ViewportSize.Y * 0.05f
	);

	const auto Movement = 1 - UKismetMathLibrary::FClamp(NormalizedMousePosition, 0.0, 1.0);
	this->Root->AddRelativeLocation(
		this->Root->GetForwardVector() * Movement * this->EdgeScrollSpeed
	);
}

void URTSCamera::EdgeScrollDown() const
{
	const auto MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this->GetWorld());
	const auto ViewportSize = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this->GetWorld()).GetLocalSize();
	const auto NormalizedMousePosition = UKismetMathLibrary::NormalizeToRange(
		MousePosition.Y,
		ViewportSize.Y * 0.95f,
		ViewportSize.Y
	);

	const auto Movement = UKismetMathLibrary::FClamp(NormalizedMousePosition, 0.0, 1.0);
	this->Root->AddRelativeLocation(
		-1 * this->Root->GetForwardVector() * Movement * this->EdgeScrollSpeed
	);
}

void URTSCamera::FollowTargetIfSet() const
{
	if (this->CameraFollowTarget != nullptr)
	{
		this->Root->SetWorldLocation(this->CameraFollowTarget->GetActorLocation());
	}
}

void URTSCamera::SmoothTargetArmLengthToDesiredZoom() const
{
	this->SpringArm->TargetArmLength = FMath::FInterpTo(
		this->SpringArm->TargetArmLength,
		this->DesiredZoomLength,
		this->DeltaSeconds,
		this->ZoomCatchupSpeed
	);
}

void URTSCamera::ConditionallyKeepCameraAtDesiredZoomAboveGround() const
{
	if (this->EnableDynamicCameraHeight)
	{
		const auto RootWorldLocation = this->Root->GetComponentLocation();
		const TArray<AActor*> ActorsToIgnore;

		if (FHitResult HitResult; UKismetSystemLibrary::LineTraceSingle(
			this->GetWorld(),
			FVector(RootWorldLocation.X, RootWorldLocation.Y, RootWorldLocation.Z + this->FindGroundTraceLength),
			FVector(RootWorldLocation.X, RootWorldLocation.Y, RootWorldLocation.Z - this->FindGroundTraceLength),
			TraceTypeQuery1,
			true,
			ActorsToIgnore,
			EDrawDebugTrace::Type::None,
			HitResult,
			true
		))
		{
			this->Root->SetWorldLocation(
				FVector(
					HitResult.Location.X,
					HitResult.Location.Y,
					HitResult.Location.Z
				)
			);
		}

		else
		{
			UKismetSystemLibrary::PrintString(
				this->GetWorld(),
				"Error: AC_RTSCamera needs to be placed on the ground! Increase trace length or change the starting position of the parent actor for the spring arm"
			);
		}
	}
}

void URTSCamera::ConditionallyApplyCameraBounds() const
{
	if (this->BoundaryVolume != nullptr)
	{
		const auto RootWorldLocation = this->Root->GetComponentLocation();
		FVector Origin;
		FVector Extents;
		this->BoundaryVolume->GetActorBounds(false, Origin, Extents);
		this->Root->SetWorldLocation(
			FVector(
				UKismetMathLibrary::Clamp(RootWorldLocation.X, Origin.X - Extents.X, Origin.X + Extents.X),
				UKismetMathLibrary::Clamp(RootWorldLocation.Y, Origin.Y - Extents.Y, Origin.Y + Extents.Y),
				RootWorldLocation.Z
			)
		);
	}
}
