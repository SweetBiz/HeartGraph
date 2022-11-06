﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetUtilsLibrary.h"

void UHeartWidgetUtilsLibrary::DrawSpline(FPaintContext& Context, const FVector2D& From, const FVector2D& FromTangent,
		const FVector2D& To, const FVector2D& ToTangent, const float Thickness, const FLinearColor& Tint)
{
	Context.MaxLayer++;

	FSlateDrawElement::MakeSpline(
		Context.OutDrawElements,
		Context.MaxLayer,
		Context.AllottedGeometry.ToPaintGeometry(),
		From,
		FromTangent,
		To,
		ToTangent,
		Thickness,
		ESlateDrawEffect::None,
		Tint);
}

void UHeartWidgetUtilsLibrary::DrawCubicBezierSpline(FPaintContext& Context, const FVector2D& P0, const FVector2D& P1,
	const FVector2D& P2, const FVector2D& P3, const float Thickness, const FLinearColor& Tint)
{
	Context.MaxLayer++;

	FSlateDrawElement::MakeCubicBezierSpline(
		Context.OutDrawElements,
		Context.MaxLayer,
		Context.AllottedGeometry.ToPaintGeometry(),
		P0,	P1,	P2,	P3,
		Thickness,
		ESlateDrawEffect::None,
		Tint);
}

FVector2D UHeartWidgetUtilsLibrary::GetWidgetCenterLocal(UWidget* Widget)
{
	if (!ensure(IsValid(Widget)))
	{
		return FVector2D::ZeroVector;
	}

	auto&& Geometry = Widget->GetCachedGeometry();
	return Geometry.GetLocalSize() * 0.5;
}

FVector2D UHeartWidgetUtilsLibrary::GetWidgetCenterAbsolute(UWidget* Widget)
{
	if (!ensure(IsValid(Widget)))
	{
		return FVector2D::ZeroVector;
	}

	auto&& Geometry = Widget->GetCachedGeometry();
	return Geometry.LocalToAbsolute(Geometry.GetLocalSize() * 0.5);
}
