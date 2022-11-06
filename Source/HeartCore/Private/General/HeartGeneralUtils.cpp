﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "General/HeartGeneralUtils.h"
#include "General/VectorBounds.h"
#include "General/Vector2DBounds.h"

TSubclassOf<UObject> UHeartGeneralUtils::GetParentClass(const TSubclassOf<UObject> Class)
{
	return IsValid(Class) ? Class->GetSuperClass() : nullptr;
}

TArray<TSubclassOf<UObject>> UHeartGeneralUtils::GetChildClasses(const TSubclassOf<UObject> Class, const bool AllowAbstract)
{
	TArray<TSubclassOf<UObject>> OutClasses;

	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		if (!(*ClassIt)->IsChildOf(Class) || ((*ClassIt)->HasAnyClassFlags(CLASS_Abstract | CLASS_NewerVersionExists) || AllowAbstract))
		{
			continue;
		}

		OutClasses.Add(*ClassIt);
	}

	return OutClasses;
}

const UObject* UHeartGeneralUtils::GetClassDefaultObject(const TSubclassOf<UObject> Class)
{
	return IsValid(Class) ? Class->GetDefaultObject() : nullptr;
}

FText UHeartGeneralUtils::GetClassDisplayNameText(const TSubclassOf<UObject> Class)
{
	return IsValid(Class) ? Class->GetDisplayNameText() : FText();
}

FText UHeartGeneralUtils::GetClassTooltip(const TSubclassOf<UObject> Class)
{
	return IsValid(Class) ? Class->GetToolTipText() : FText();
}

FVector UHeartGeneralUtils::BP_ClampVector(const FVector& Value, const FVectorBounds& Bounds)
{
	return ClampVector(Value, Bounds.Min, Bounds.Max);
}

FVector2D UHeartGeneralUtils::BP_ClampVector2D(const FVector2D& Value, const FVector2DBounds& Bounds)
{
	return FVector2D(FMath::Clamp(Value.X, Bounds.Min.X, Bounds.Max.X),
					 FMath::Clamp(Value.Y, Bounds.Min.Y, Bounds.Max.Y));
}
