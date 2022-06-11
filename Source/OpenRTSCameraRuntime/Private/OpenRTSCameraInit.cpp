// Copyright 2022 Jesus Bracho All Rights Reserved.

#include "OpenRTSCameraInit.h"

#include "Kismet/KismetSystemLibrary.h"

UOpenRTSCameraInit::UOpenRTSCameraInit()
{
	
}

void UOpenRTSCameraInit::BeginPlay()
{
	Super::BeginPlay();

	UKismetSystemLibrary::PrintString(
		this->GetWorld(),
		"UOpenRTSCameraInit::BeginPlay()"
	);

	
}
