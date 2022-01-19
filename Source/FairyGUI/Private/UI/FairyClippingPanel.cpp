#include "UI/FairyClippingPanel.h"
#include "FairyApplication.h"

UFairyClippingPanel::UFairyClippingPanel()
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
	}
}

UFairyClippingPanel::~UFairyClippingPanel()
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
	}
}

void UFairyClippingPanel::SetupClipping()
{
	GetDisplayObject()->SetClipping(EWidgetClipping::ClipToBoundsAlways);
	GetDisplayObject()->SetCullingBoundsExtension(Margin);
}
