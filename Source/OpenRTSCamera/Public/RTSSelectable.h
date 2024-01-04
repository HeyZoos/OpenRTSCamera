#pragma once
#include "RTSSelectable.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OPENRTSCAMERA_API URTSSelectable : public UActorComponent
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnSelected();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnDeselected();
};
