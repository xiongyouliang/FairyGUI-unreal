#include "Event/EventContext.h"

void UEventContext::Reset()
{
	Sender = nullptr;
	Initiator = nullptr;

	PointerEvent = nullptr;
	KeyEvent = nullptr;

	Type = FFairyEventNames::Null;
	bStopped = false;
	bDefaultPrevented = false;
	bIsMouseCaptor = false;
	Data.Reset();
}