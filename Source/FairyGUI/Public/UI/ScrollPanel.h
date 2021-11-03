#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateTypes.h"
#include "FieldTypes.h"
#include "Tween/GTween.h"
#include "Event/EventContext.h"
#include "ScrollPanel.generated.h"

class UFairyObject;
class UFairyComponent;
class UGScrollBar;
class UGController;
class FByteBuffer;
class FGTweener;
class SContainer;

UCLASS(BlueprintType)
class FAIRYGUI_API UScrollPanel : public UObject
{
    GENERATED_BODY()
public:
    UScrollPanel();
    ~UScrollPanel();

    void Setup(FByteBuffer* Buffer);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    UFairyComponent* GetHeader() const { return Header; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    UFairyComponent* GetFooter() const { return Footer; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    UGScrollBar* GetVtScrollBar() const { return VScrollBar; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    UGScrollBar* GetHzScrollBar() const { return HScrollBar; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    float GetPosX() const { return XPos; }
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetPosX(float Value, bool bAnimation = false);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    float GetPosY() const { return YPos; }
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetPosY(float Value, bool bAnimation = false);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    float GetPercX() const;
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetPercX(float Value, bool bAnimation = false);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    float GetPercY() const;
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetPercY(float Value, bool bAnimation = false);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool IsBottomMost() const;

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool IsRightMost() const;

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void ScrollLeft(float Ratio = 1, bool bAnimation = false);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void ScrollRight(float Ratio = 1, bool bAnimation = false);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void ScrollUp(float Ratio = 1, bool bAnimation = false);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void ScrollDown(float Ratio = 1, bool bAnimation = false);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void ScrollTop(bool bAnimation = false);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void ScrollBottom(bool bAnimation = false);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void ScrollToView(UFairyObject* Obj, bool bAnimation = false, bool bSetFirst = false);

    void ScrollToView(const FBox2D& Rect, bool bAnimation = false, bool bSetFirst = false);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool IsChildInView(UFairyObject* Obj) const;

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    int32 GetPageX() const;
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetPageX(int32 PageX, bool bAnimation = false);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    int32 GetPageY() const;
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetPageY(int32 PageY, bool bAnimation = false);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    float GetScrollingPosX() const;
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    float GetScrollingPosY() const;

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    const FVector2D& GetContentSize() const { return ContentSize; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    const FVector2D& GetViewSize() const { return ViewSize; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void LockHeader(int32 Size);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void LockFooter(int32 Size);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void CancelDragging();

    static UScrollPanel* GetDraggingPane() { return DraggingPane.Get(); }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    uint8 bBouncebackEffect : 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    uint8 bTouchScrollable : 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    uint8 bInertiaDisabled : 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    uint8 bMouseWheelEnabled : 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    uint8 bSnapToItem : 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    uint8 bPageMode : 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    float DecelerationRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    float ScrollStep;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    UGController* PageController;

private:
    void OnOwnerSizeChanged();
    void SetContentSize(const FVector2D& InSize);
    void ChangeContentSizeOnScrolling(float DeltaWidth, float DeltaHeight, float DeltaPosX, float DeltaPosY);
    void SetViewWidth(float Width);
    void SetViewHeight(float Height);
    void SetSize(const FVector2D& InSize);
    void HandleSizeChanged();

    void ApplyController(UGController* Controller);
    void UpdatePageController();

    void PosChanged(bool bAnimation);
    void Refresh();
    void Refresh2();

    void UpdateScrollBarPos();
    void UpdateScrollBarVisible();
    void UpdateScrollBarVisible2(UGScrollBar* Bar);

    float GetLoopPartSize(float Division, int32 Axis);
    bool LoopCheckingCurrent();
    void LoopCheckingTarget(FVector2D& EndPos);
    void LoopCheckingTarget(FVector2D& EndPos, int32 Axis);
    void LoopCheckingNewPos(float& Value, int32 Axis);
    void AlignPosition(FVector2D& Pos, bool bInertialScrolling);
    float AlignByPage(float Pos, int32 Axis, bool bInertialScrolling);
    FVector2D UpdateTargetAndDuration(const FVector2D& OrignPos);
    float UpdateTargetAndDuration(float Pos, int32 Axis);
    void FixDuration(int32 Axis, float DldChange);
    void StartTween(int32 Type);
    void KillTween();
    void TweenUpdate();
    float RunTween(int32 Axis, float Delta);

    void CheckRefreshBar();

    void OnTouchBegin(UEventContext* Context);
    void OnTouchMove(UEventContext* Context);
    void OnTouchEnd(UEventContext* Context);
    void OnMouseWheel(UEventContext* Context);
    void OnRollOver(UEventContext* Context);
    void OnRollOut(UEventContext* Context);

    inline void LimitContainerPos();

    void OnBarTweenComplete(FGTweener* Tweener);

private:
    UFairyComponent* Owner;
    TSharedPtr<SContainer> OwnerRootContainer;
    TSharedPtr<SContainer> OwnerMaskContainer;
    TSharedPtr<SContainer> OwnerContentContainer;

    UPROPERTY(Transient)
    UGScrollBar* HScrollBar;
    UPROPERTY(Transient)
    UGScrollBar* VScrollBar;
    UPROPERTY(Transient)
    UFairyComponent* Header;
    UPROPERTY(Transient)
    UFairyComponent* Footer;

    void AddCommonWidget(UFairyObject*);

    EFairyScrollDirection ScrollDirection;
    FMargin ScrollBarMargin;
    uint8 bScrollBarDisplayAuto : 1;
    uint8 bVScrollNone : 1;
    uint8 bHScrollNone : 1;
    uint8 bNeedRefresh : 1;
    int32 RefreshBarAxis;
    uint8 bScrollBarOnLeft : 1; // show scroll bar on left;
    uint8 bScrollBarDisplayInDemand : 1; // show scroll bar just when content size bigger then view size;
    uint8 bFloating : 1;
    uint8 bDontClipMargin : 1;

    float XPos;
    float YPos;

    FVector2D ViewSize; // the scroll view size, same value with mask widget size.
    FVector2D ContentSize; // the scroll content container widget size.
    FVector2D ContainerPos; // the scroll content container widget position.
    FVector2D OverlapSize;
    FVector2D PageSize;


    FVector2D BeginTouchPos; // cache touch position at begin event
    FVector2D ContainerPosAtBegin; // cache position of scroll container at begin event
    FVector2D LastTouchPos;
    FVector2D LastTouchGlobalPos;

    FVector2D Velocity;
    float VelocityScale;
    float LastMoveTime;
    uint8 bDragged : 1;
    uint8 bIsHoldAreaDone : 1;
    int32 AniFlag;
    int32 LoopMode;
    uint8 bHover : 1;

    float HeaderLockedSize;
    float FooterLockedSize;
    uint8 bDispatchingPullDown : 1;
    uint8 bDispatchingPullUp : 1;

    int32 TweenType;
    FVector2D TweenStart;
    FVector2D TweenChange;
    FVector2D TweenTime;
    FVector2D TweenDuration;

    FTimerHandle RefreshTimerHandle;
    FTimerHandle TickTimerHandle;

    static TWeakObjectPtr<UScrollPanel> DraggingPane;

    friend class UFairyComponent;
    friend class UGList;
    friend class UGScrollBar;
};