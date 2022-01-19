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
    virtual void ConstructExtension(FairyGUI::FByteBuffer* Buffer) override;

private:
    void OnTouchBeginHandler(UEventContext* Context);
    void OnGripTouchBegin(UEventContext* Context);
    void OnGripTouchMove(UEventContext* Context);
    void OnGripTouchEnd(UEventContext* Context);
    void OnArrowButton1Click(UEventContext* Context);
    void OnArrowButton2Click(UEventContext* Context);

    UPROPERTY()
    UFairyObject* GripObject;
    UPROPERTY()
    UFairyObject* ArrowButton1;
    UPROPERTY()
    UFairyObject* ArrowButton2;
    UPROPERTY()
    UFairyObject* BarObject;
    UPROPERTY()
    UScrollPanel* Target;

    bool bVertical;
    float ScrollPerc;
    bool bFixedGripSize;

    FVector2D DragOffset;
};