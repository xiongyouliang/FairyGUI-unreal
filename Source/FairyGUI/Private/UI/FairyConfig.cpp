#include "UI/FairyConfig.h"
#include "FairyApplication.h"

UFairyConfig* UFairyConfig::Config = nullptr;

UFairyConfig::UFairyConfig() :
    ButtonSoundVolumeScale(1),
    DefaultScrollStep(25),
    DefaultScrollDecelerationRate(0.967f),
    DefaultScrollTouchEffect(true),
    DefaultScrollBounceEffect(true),
    DefaultScrollBarDisplay(EScrollBarDisplayType::Default),
    TouchDragSensitivity(10),
    ClickDragSensitivity(2),
    TouchScrollSensitivity(20),
    DefaultComboBoxVisibleItemCount(10),
    ModalLayerColor(0, 0, 0, 120),
    BringWindowToFrontOnClick(true)
{
}

void UFairyConfig::Create(UObject* owner)
{
	if (UFairyConfig::Config == nullptr)
	{
		UFairyConfig::Config = NewObject<UFairyConfig>(owner);
        UFairyConfig::Config->AddToRoot();
	}
}

UFairyConfig* UFairyConfig::Get()
{
    if (UFairyConfig::Config == nullptr)
    {
        UFairyConfig::Create(UFairyApplication::Get());
    }
    return UFairyConfig::Config;
}