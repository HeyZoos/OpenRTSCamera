// Copyright 2022, Jesus Bracho, All Rights Reserved.

#include "RTSCameraBoundsVolume.h"
#include "Components/PrimitiveComponent.h"

ARTSCameraBoundsVolume::ARTSCameraBoundsVolume()
{
    this->Tags.Add("OpenRTSCamera#CameraBounds");
    
    if (UPrimitiveComponent* PrimitiveComponent = this->FindComponentByClass<UPrimitiveComponent>())
    {
        PrimitiveComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName, false);
    }
}
