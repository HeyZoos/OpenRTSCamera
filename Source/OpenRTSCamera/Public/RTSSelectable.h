#pragma once
#include "RTSSelectable.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OPENRTSCAMERA_API URTSSelectable : public UActorComponent
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "RTS Selection")
	void OnSelected();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "RTS Selection")
	void OnDeselected();
};
