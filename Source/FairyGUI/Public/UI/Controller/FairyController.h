#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Delegates/Delegate.h"
#include "Delegates/DelegateCombinations.h"
#include "UI/Controller/Actions/ControllerAction.h"
#include "Utils/ByteBuffer.h"
#include "UI/Controller/Gears/FairyGearType.h"

#include "FairyController.generated.h"

class UFairyObject;
class UFairyComponent;
class UFairyController;

enum class EHomePageType : int32
{
	FirstPage = 0, // default home page type
	Specific,
	MatchBranch,
	MatchVariable
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyController : public UObject
{
	GENERATED_BODY()
public:
	UFairyController();
	virtual ~UFairyController();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FName GetName() { return Name; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	int32 GetSelectedIndex() const { return SelectedIndex; }
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetSelectedIndex(int32 Index);
	void SetSelectedIndex(int32 Index, bool bTriggerEvent);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FName GetSelectedPage() const;

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetSelectedPage(const FName& PageName);
	void SetSelectedPage(const FName& PageName, bool bTriggerEvent);

	FName GetSelectedPageID() const;
	void SetSelectedPageID(const FName& PageID, bool bTriggerEvent = true);

	int32 GetPrevisousIndex() const { return PreviousIndex; }
	FName GetPreviousPage() const;
	FName GetPreviousPageID() const;

	int32 GetPageCount() const;
	bool HasPage(const FName& PageName) const;
	int32 GetPageIndexByID(const FName& PageID) const;
	FName GetPageNameByID(const FName& PageID) const;
	FName GetPageID(int32 Index) const;
	void SetOppositePageID(const FName& PageID);

	void AddAbserver(TSharedPtr<FGearBase> InPenddingAddGear);
	void RemoveAllAbserver();
	void RemoveAbserver(TSharedPtr<FGearBase> InPenddingRemoveGear);
	void Apply();
	bool CheckGearVisible(const UFairyObject* InTarget);

	void RunActions();

	// Initialize from binary data
	void Setup(FairyGUI::FByteBuffer* Buffer);

	FName Name;
	bool bChanging;
	bool bAutoRadioGroupDepth;

private:
	int32 SelectedIndex;
	int32 PreviousIndex;

	TArray<FName> PageIDs;
	TArray<FName> PageNames;

	TIndirectArray<FControllerAction> Actions;
	TArray<TSharedPtr<FGearBase>> ObserverList;
};
