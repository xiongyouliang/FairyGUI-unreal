#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FieldTypes.generated.h"

UENUM()
enum class EPackageItemType
{
	Image,
	MovieClip,
	Sound,
	Component,
	Atlas,
	Font,
	Swf,
	Misc,
	Unknown,
	Spine,
	DragonBones
};

UENUM()
enum class EObjectType
{
	Image,
	MovieClip,
	Swf,
	Graph,
	Loader,
	Group,
	Text,
	RichText,
	InputText,
	Component,
	List,
	Label,
	Button,
	ComboBox,
	ProgressBar,
	Slider,
	ScrollBar,
	Tree,
	Loader3D
};

enum class EComponentSubtype
{
	Controller,
	Clipping,
	ScrollView,
	List
};

UENUM()
enum class EButtonMode : uint8
{
	Common,
	Check,
	Radio
};

UENUM()
enum class EButtonPressedEffect : uint8
{
	None = 0,
	Dark = 1,
	Scale = 2
};

UENUM()
enum class EChildrenRenderOrder : uint8
{
	Ascent,
	Descent,
	Arch,
};

UENUM()
enum class EOverflowType : uint8
{
	Visible,
	Hidden,
	Scroll
};

UENUM()
enum class EFairyScrollDirection : uint8
{
	Horizontal,
	Vertical,
	Both
};

UENUM()
enum class EScrollBarDisplayType : uint8
{
	Default,
	Visible,
	Auto,
	Hidden
};

UENUM()
enum class ELoaderFillType : uint8
{
	None,
	Scale,
	ScaleMatchHeight,
	ScaleMatchWidth,
	ScaleFree,
	ScaleNoBorder
};

UENUM()
enum class EProgressTitleType : uint8
{
	Percent,
	ValueMax,
	Value,
	Max
};

UENUM()
enum class EListLayoutType : uint8
{
	SingleCol,
	SingleRow,
	HorizontalFlow,
	VerticalFlow,
	Pagination
};

UENUM()
enum class EListSelectionMode : uint8
{
	Single,
	Multiple,
	MultipleSingleclick,
	None
};

UENUM()
enum class EGroupLayoutType : uint8
{
	None,
	Horizontal,
	Vertical
};

UENUM()
enum class EPopupDirection : uint8
{
	Auto,
	Up,
	Down
};

UENUM()
enum class EAutoSizeType : uint8
{
	None,
	Both,
	Height,
	Shrink
};

UENUM()
enum class EFlipType : uint8
{
	None,
	Horizontal,
	Vertical,
	Both
};

UENUM()
enum class ETransitionActionType
{
	XY,
	Size,
	Scale,
	Pivot,
	Alpha,
	Rotation,
	Color,
	Animation,
	Visible,
	Sound,
	Transition,
	Shake,
	ColorFilter,
	Skew,
	Text,
	Icon,
	Unknown
};

UENUM()
enum class EFillMethod : uint8
{
	None,
	Horizontal,
	Vertical,
	Radial90,
	Radial180,
	Radial360,
};

UENUM()
enum class EOriginHorizontal : uint8
{
	Left,
	Right,
};

UENUM()
enum class EOriginVertical : uint8
{
	Top,
	Bottom
};

UENUM()
enum class EOrigin90 : uint8
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight
};

UENUM()
enum class EOrigin180 : uint8
{
	Top,
	Bottom,
	Left,
	Right
};

UENUM()
enum class EOrigin360 : uint8
{
	Top,
	Bottom,
	Left,
	Right
};

UENUM()
enum class EObjectPropID
{
	Text,
	Icon,
	Color,
	OutlineColor,
	Playing,
	Frame,
	DeltaTime,
	TimeScale,
	FontSize,
	Selected
};

UENUM()
enum class EHAlignType : uint8
{
	Left,
	Center,
	Right
};

UENUM()
enum class EVAlignType : uint8
{
	Top,
	Middle,
	Bottom
};

namespace ERelation
{
	enum PosDirection
	{
		NoPosDirection,
		Horizontal,
		Vertical
	};

	enum ExtDirection
	{
		NoExtDirection,
		Width,
		Height
	};
};

UENUM()
enum class ERelationType : uint8
{
	// Position Relation
	// horizontal position relations, for a specific target object only has one ERelationType
	Left_Left,
	Left_Center,
	Left_Right,
	Center_Center,
	Right_Left,
	Right_Center,
	Right_Right,

	// vertical position relations
	Top_Top,
	Top_Middle,
	Top_Bottom,
	Middle_Middle,
	Bottom_Top,
	Bottom_Middle,
	Bottom_Bottom,


	// Extension Relation
	Width,
	Height,

	LeftExt_Left,
	LeftExt_Right,
	RightExt_Left,
	RightExt_Right,
	TopExt_Top,
	TopExt_Bottom,
	BottomExt_Top,
	BottomExt_Bottom,

	Size
};