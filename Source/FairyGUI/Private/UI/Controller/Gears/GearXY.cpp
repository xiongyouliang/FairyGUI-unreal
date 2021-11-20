#include "UI/Controller/Gears/GearXY.h"
#include "UI/FairyComponent.h"
#include "Package/FairyPackage.h"
#include "UI/Controller/GController.h"
#include "Utils/ByteBuffer.h"

FGearXY::FGearXY(UFairyObject* InOwner) :
    FGearBase(InOwner),
    bPositionsInPercent(false)
{
    Type = EGearType::XY;
}

FGearXY::~FGearXY()
{
}

void FGearXY::Init()
{
    const FVector2D OwnerPos = Owner->GetPosition();
    Default = FVector4(OwnerPos.X, OwnerPos.Y,
        OwnerPos.X / Owner->GetParent()->GetWidth(),
        OwnerPos.Y / Owner->GetParent()->GetHeight());
    Storage.Reset();
}

void FGearXY::AddStatus(const FString& PageID, FByteBuffer* Buffer)
{
    FVector4 Value;
    Value.X = Buffer->ReadInt();
    Value.Y = Buffer->ReadInt();

    if (PageID.IsEmpty())
    {
		Default = Value;
    }
    else
    {
		Storage.Add(PageID, MoveTemp(Value));
    }
}

void FGearXY::AddExtStatus(const FString& PageID, FByteBuffer* Buffer)
{
    FVector4* Value = PageID.IsEmpty() ? &Default : Storage.Find(Controller->GetSelectedPageID());
    Value->Z = Buffer->ReadFloat();
    Value->W = Buffer->ReadFloat();
}

void FGearXY::Apply()
{
    FVector4* Value = Storage.Find(Controller->GetSelectedPageID());
    if (Value == nullptr)
    {
		Value = &Default;
    }

    FVector2D EndPt;

    if (bPositionsInPercent && Owner->GetParent())
    {
        EndPt.X = Value->Z * Owner->GetParent()->GetWidth();
        EndPt.Y = Value->W * Owner->GetParent()->GetHeight();
    }
    else
    {
        EndPt.X = Value->X;
        EndPt.Y = Value->Y;
    }

    if (TweenConfig.IsSet() && TweenConfig->bTween && UFairyPackage::Constructing == 0 && !bDisableAllTweenEffect)
    {
        // todo: use new tween system.
    }
    else
    {
        Owner->bGearLocked = true;
        Owner->SetPosition(EndPt);
        Owner->bGearLocked = false;
    }
}

void FGearXY::OnTweenComplete()
{
    if (TweenConfig->DisplayLockToken != 0)
    {
        Owner->ReleaseDisplayLock(TweenConfig->DisplayLockToken);
        TweenConfig->DisplayLockToken = 0;
    }
    TweenConfig->Handle.Invalidate();
    Owner->DispatchEvent(FFairyEventNames::GearStop);
}

void FGearXY::UpdateState()
{
    const FVector2D& OwnerPos = Owner->GetPosition();
    Storage.Add(Controller->GetSelectedPageID(), FVector4(
        OwnerPos.X, OwnerPos.Y,
        OwnerPos.X / Owner->GetParent()->GetWidth(),
        OwnerPos.Y / Owner->GetParent()->GetHeight()));
}

void FGearXY::UpdateFromRelations(const FVector2D& Delta)
{
    if (Controller.Get() != nullptr && Storage.Num() > 0 && !bPositionsInPercent)
    {
        for (auto It = Storage.CreateIterator(); It; ++It)
        {
            It->Value = FVector4(It->Value.X + Delta.X, It->Value.Y + Delta.Y, It->Value.Z, It->Value.W);
        }
        Default.X += Delta.X;
        Default.Y += Delta.Y;

        UpdateState();
    }
}
