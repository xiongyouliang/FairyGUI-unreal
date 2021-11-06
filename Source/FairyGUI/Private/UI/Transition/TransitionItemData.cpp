#include "UI/Transition/TransitionItemData.h"

FTransitionItemData::FTransitionItemData() {
  f1 = f2 = f3 = f4 = 0;
  b1 = b2 = true;
  b3 = false;
}

FVector2D FTransitionItemData::GetVec2() const { return FVector2D(f1, f2); }

void FTransitionItemData::SetVec2(const FVector2D &value) {
  f1 = value.X;
  f2 = value.Y;
}

FVector4 FTransitionItemData::GetVec4() const {
  return FVector4(f1, f2, f3, f4);
}

void FTransitionItemData::SetVec4(const FVector4 &value) {
  f1 = value.X;
  f2 = value.Y;
  f3 = value.Z;
  f4 = value.W;
}

FColor FTransitionItemData::GetColor() const {
  return FColor(f1 * 255.f, f2 * 255.f, f3 * 255.f, f4 * 255.f);
}

void FTransitionItemData::SetColor(const FColor &value) {
  f1 = value.R / 255.f;
  f2 = value.G / 255.f;
  f3 = value.B / 255.f;
  f4 = value.A / 255.f;
}