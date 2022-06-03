// Copyright 2022 Jesus Bracho All Rights Reserved.


#include "CameraBoundsVolume.h"

ACameraBoundsVolume::ACameraBoundsVolume()
{
	this->Tags.Add(FName("OpenRTSCamera#CameraBounds"));
	this
		->FindComponentByClass<UPrimitiveComponent>()
		->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}
