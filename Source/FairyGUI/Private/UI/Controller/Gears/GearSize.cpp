#include "UI/Controller/Gears/GearSize.h"
#include "UI/FairyObject.h"
#include "Package/FairyPackage.h"
#include "UI/Controller/GController.h"
#include "Utils/ByteBuffer.h"

FGearSize::FGearSize(UFairyObject* InOwner) : FGearBase(InOwner)
{
    Type = EGearType::Size;
}

FGearSize::~FGearSize()
{
}

void FGearSize::Init()
{
    Default = FVector4(Owner->GetWidth(), Owner->GetHeight(), Owner->GetScaleX(), Owner->GetScaleY());
    Storage.Reset();
}

void FGearSize::AddStatus(const FString& PageID, FByteBuffer* Buffer)
{
    FVector4 Value;
    Value.X = Buffer->ReadInt();
    Value.Y = Buffer->ReadInt();
    Value.Z = Buffer->ReadFloat();
    Value.W = Buffer->ReadFloat();

    if (PageID.IsEmpty())
    {
        Default = Value;
    }
    else
    {
        Storage.Add(PageID, MoveTemp(Value));
    }
}

void FGearSize::Apply()
{
    FVector4* Value = Storage.Find(Controller->GetSelectedPageID());
    if (Value == nullptr)
    {
        Value = &Default;
    }

    if (TweenConfig.IsSet() && TweenConfig->bTween && UFairyPackage::Constructing == 0 && !bDisableAllTweenEffect)
    {
        // todo: use new tween system
    }
    else
    {
        Owner->bGearLocked = true;
        Owner->SetSize(FVector2D(Value->X, Value->Y));
        Owner->SetPivot(Owner->GetPivot(), Owner->CheckGearController(1, Controller.Get()));
        Owner->SetScale(FVector2D(Value->Z, Value->W));
        Owner->bGearLocked = false;
    }
}

void FGearSize::OnTweenUpdate(UFairyTweener* Tweener)
{
    int32 flag = Tweener->GetUserData().AsInt();
    Owner->bGearLocked = true;
    if ((flag & 1) != 0)
    {
        Owner->SetSize(Tweener->Value.GetVec2());
        Owner->SetPivot(Owner->GetPivot(), Owner->CheckGearController(1, Controller.Get()));
    }
        
    if ((flag & 2) != 0)
    {
        Owner->SetScale(FVector2D(Tweener->Value.Z, Tweener->Value.W));
    }
    Owner->bGearLocked = false;
}

void FGearSize::OnTweenComplete()
{
    if (TweenConfig->DisplayLockToken != 0)
    {
        Owner->ReleaseDisplayLock(TweenConfig->DisplayLockToken);
        TweenConfig->DisplayLockToken = 0;
    }
    TweenConfig->Handle.Invalidate();
    Owner->DispatchEvent(FFairyEventNames::GearStop);
}

void FGearSize::UpdateState()
{
    Storage.Add(Controller->GetSelectedPageID(), FVector4(Owner->GetWidth(), Owner->GetHeight(),
        Owner->GetScaleX(), Owner->GetScaleY()));
}

void FGearSize::UpdateFromRelations(const FVector2D& Delta)
{
    if (Controller != nullptr && Storage.Num() > 0)
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
