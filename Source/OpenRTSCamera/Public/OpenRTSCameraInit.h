// Copyright 2022 Jesus Bracho All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OpenRTSCameraInit.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OPENRTSCAMERARUNTIME_API UOpenRTSCameraInit : public UActorComponent
{
	GENERATED_BODY()

public:
	UOpenRTSCameraInit();
};
