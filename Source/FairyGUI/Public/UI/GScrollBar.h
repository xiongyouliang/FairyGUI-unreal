#pragma once

#include "UI/FairyComponent.h"
#include "GScrollBar.generated.h"

class UFairyScrollPane;

UCLASS(BlueprintType, Blueprintable)
class FAIRYGUI_API UGScrollBar : public UFairyComponent
{
    GENERATED_BODY()

public:
    UGScrollBar();
    virtual ~UGScrollBar();

    void SetScrollPane(UScrollPanel* Target, bool bVertical);
    void SetDisplayPerc(float Value);
    void SetScrollPerc(float Value);
    float GetMinSize();

    bool bGripDragging;

protected:
    virtual void ConstructExtension(FByteBuffer* Buffer);

private:
    void OnTouchBeginHandler(UEventContext* Context);
    void OnGripTouchBegin(UEventContext* Context);
    void OnGripTouchMove(UEventContext* Context);
    void OnGripTouchEnd(UEventContext* Context);
    void OnArrowButton1Click(UEventContext* Context);
    void OnArrowButton2Click(UEventContext* Context);

    UFairyObject* GripObject;
    UFairyObject* ArrowButton1;
    UFairyObject* ArrowButton2;
    UFairyObject* BarObject;
    UScrollPanel* Target;

    bool bVertical;
    float ScrollPerc;
    bool bFixedGripSize;

    FVector2D DragOffset;
};