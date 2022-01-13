#pragma once

#include "UI/FairyObject.h"
#include "UI/FairyComponent.h"
#include "UI/ScrollPanel.h"

#include "FairyClippingPanel.generated.h"

class UFairyController;
class UFairyTransition;
class SContainer;

UCLASS(BlueprintType, Blueprintable)
class FAIRYGUI_API UFairyClippingPanel : public UFairyComponent
{
	GENERATED_BODY()
public:
	UFairyClippingPanel();
	virtual ~UFairyClippingPanel();
};