// Copyright 2022 Jesus Bracho All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CameraBlockingVolume.h"
#include "CameraBoundsVolume.generated.h"

UCLASS()
class OPENRTSCAMERA_API ACameraBoundsVolume : public ACameraBlockingVolume
{
	GENERATED_BODY()
	ACameraBoundsVolume();	
};
