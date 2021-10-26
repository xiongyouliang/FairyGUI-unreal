#pragma once

#include "UI/FairyComponent.h"
#include "GWindow.generated.h"

class FAIRYGUI_API IUISource
{
public:
    virtual const FString& GetFileName() = 0;
    virtual void SetFileName(const FString& InFileName) = 0;
    virtual bool IsLoaded() = 0;
    virtual void Load(FSimpleDelegate Callback) = 0;
};

DECLARE_DYNAMIC_DELEGATE(FWindowDynDelegate);

UCLASS(BlueprintType, Blueprintable)
class FAIRYGUI_API UGWindow : public UFairyComponent
{
    GENERATED_BODY()
public:
    UGWindow();
    virtual ~UGWindow();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void Show();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void Hide();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void HideImmediately();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void ToggleStatus();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void BringToFront();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool IsShowing() const { return Parent != nullptr; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool IsTop() const;

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool IsModal() const { return bModal; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetModal(bool bInModal) { bModal = bInModal; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void ShowModalWait(int32 InRequestingCmd = 0);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool CloseModalWait(int32 InRequestingCmd = 0);

    void Init();
    void AddUISource(TSharedPtr<IUISource> UISource);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    UFairyComponent* GetContentPane() const { return ContentPane; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetContentPane(UFairyComponent* Obj);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    UFairyComponent* GetFrame() const { return FrameObject; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    UFairyObject* GetCloseButton() const { return CloseButton; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetCloseButton(UFairyObject* Obj);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    UFairyObject* GetDragArea() const { return DragArea; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetDragArea(UFairyObject* Obj);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    UFairyObject* GetContentArea() const { return ContentArea; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetContentArea(UFairyObject* Obj) { ContentArea = Obj; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    UFairyObject* GetModalWaitingPane() const { return ModalWaitPane; }

    virtual void OnShown();
    virtual void OnHide();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    bool bBringToFontOnClick;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    FWindowDynDelegate InitCallback;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    FWindowDynDelegate ShownCallback;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    FWindowDynDelegate HideCallback;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    FWindowDynDelegate ShowingCallback;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    FWindowDynDelegate HidingCallback;

protected:
    virtual void OnInit();
    virtual void DoShowAnimation();
    virtual void DoHideAnimation();

    UFUNCTION()
    void CloseEventHandler(UEventContext* Context);

    UPROPERTY(Transient)
    UFairyComponent* ContentPane;

private:
    void LayoutModalWaitPane();
    void OnUILoadComplete();
    void InternalInit();

    void OnTouchBeginHandler(UEventContext* Context);
    UFUNCTION()
    void OnDragStartHandler(UEventContext* Context);
    void OnAddedToStageHandler(UEventContext* Context);
    void OnRemovedFromStageHandler(UEventContext* Context);

    UPROPERTY(Transient)
    UFairyObject* ModalWaitPane;
    UPROPERTY(Transient)
    UFairyComponent* FrameObject;
    UPROPERTY(Transient)
    UFairyObject* CloseButton;
    UPROPERTY(Transient)
    UFairyObject* DragArea;
    UPROPERTY(Transient)
    UFairyObject* ContentArea;

    int32 RequestingCmd;
    TArray<TSharedPtr<IUISource>> UISources;
    uint8 bModal : 1;
    uint8 bInited : 1;
    uint8 bLoading : 1;
};