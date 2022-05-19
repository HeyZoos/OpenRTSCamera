// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "Engine/BlockingVolume.h"
#include "GameFramework/SpringArmComponent.h"
#include "RTSCamera.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OPENRTSCAMERA_API URTSCamera : public UActorComponent
{
	GENERATED_BODY()

public:
	URTSCamera();

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	UFUNCTION(BlueprintCallable)
	void FollowTarget(AActor* Target);

	UFUNCTION(BlueprintCallable)
	void UnFollowTarget();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName CameraBlockingVolumeTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Zoom Settings")
	float MinimumZoomLength;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Zoom Settings")
	float MaximumZoomLength;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Zoom Settings")
	float ZoomCatchupSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Zoom Settings")
	float ZoomSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double StartingYAngle;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double StartingZAngle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MoveSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RotateSpeed;


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool EnableCameraLag;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool EnableCameraRotationLag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Dynamic Camera Height Settings")
	bool EnableDynamicCameraHeight;
	UPROPERTY(
		BlueprintReadWrite,
		EditAnywhere,
		Category = "RTSCamera - Dynamic Camera Height Settings",
		meta=(EditCondition="EnableDynamicCameraHeight")
	)
	float FindGroundTraceLength;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Edge Scroll Settings")
	bool EnableEdgeScrolling;
	UPROPERTY(
		BlueprintReadWrite,
		EditAnywhere,
		Category = "RTSCamera - Edge Scroll Settings",
		meta=(EditCondition="EnableEdgeScrolling")
	)
	float EdgeScrollSpeed;
	UPROPERTY(
		BlueprintReadWrite,
		EditAnywhere,
		Category = "RTSCamera - Edge Scroll Settings",
		meta=(EditCondition="EnableEdgeScrolling")
	)
	float DistanceFromEdgeThreshold;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Inputs")
	UInputMappingContext* InputMappingContext;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Inputs")
	UInputAction* RotateCameraAxis;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Inputs")
	UInputAction* TurnCameraLeft;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Inputs")
	UInputAction* TurnCameraRight;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Inputs")
	UInputAction* MoveCameraYAxis;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Inputs")
	UInputAction* MoveCameraXAxis;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Inputs")
	UInputAction* ZoomCamera;

protected:
	virtual void BeginPlay() override;

	void OnZoomCamera(const FInputActionValue& Value);
	void OnRotateCamera(const FInputActionValue& Value);
	void OnTurnCameraLeft(const FInputActionValue& Value);
	void OnTurnCameraRight(const FInputActionValue& Value);
	void OnMoveCameraYAxis(const FInputActionValue& Value);
	void OnMoveCameraXAxis(const FInputActionValue& Value);

	void MoveSpringArmWithMoveSpeed(float X, float Y, float Scale) const;

	UPROPERTY()
	AActor* Owner;
	UPROPERTY()
	USceneComponent* Root;
	UPROPERTY()
	UCameraComponent* Camera;
	UPROPERTY()
	USpringArmComponent* SpringArm;
	UPROPERTY()
	APlayerController* PlayerController;
	UPROPERTY()
	ABlockingVolume* BoundaryVolume;

	float DesiredZoomLength;

private:
	void CollectComponentDependencyReferences();
	void ConfigureSpringArm();
	void TryToFindBoundaryVolumeReference();
	void ConditionallyEnableEdgeScrolling() const;
	void CheckForEnhancedInputComponent() const;	
	void BindInputMappingContext() const;
	void BindInputActions();

	void ConditionallyPerformEdgeScrolling() const;
	void EdgeScrollLeft() const;
	void EdgeScrollRight() const;
	void EdgeScrollUp() const;
	void EdgeScrollDown() const;

	void FollowTargetIfSet() const;
	void SmoothTargetArmLengthToDesiredZoom() const;
	void ConditionallyKeepCameraAtDesiredZoomAboveGround() const;
	void ConditionallyApplyCameraBounds() const;

	UPROPERTY()
	float DeltaSeconds;
	UPROPERTY()
	AActor* CameraFollowTarget;
};
