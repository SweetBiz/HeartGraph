﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "General/HeartGeneralUtils.h"
#include "General/VectorBounds.h"
#include "General/Vector2DBounds.h"

#include "Algo/LevenshteinDistance.h"
#include "EngineFontServices.h"
#include "Engine/Font.h"
#include "Fonts/FontCache.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGeneralUtils)

UObject* UHeartGeneralUtils::K2_DuplicateObject(UObject* Outer, UObject* Source)
{
	return DuplicateObject(Source, Outer);
}

UClass* UHeartGeneralUtils::GetParentClass(const UClass* Class)
{
	return IsValid(Class) ? Class->GetSuperClass() : nullptr;
}

TArray<UClass*> UHeartGeneralUtils::GetChildClasses(const UClass* BaseClass, const bool AllowAbstract)
{
	TArray<UClass*> OutClasses;
	GetDerivedClasses(BaseClass, OutClasses);

	if (!AllowAbstract)
	{
		OutClasses.RemoveAll(
			[](const UClass* Class)
			{
				return Class->HasAnyClassFlags(CLASS_Abstract);
			});
	}

	return OutClasses;
}

UObject* UHeartGeneralUtils::ClassAsObject(UClass* Class)
{
	return Class;
}

UClass* UHeartGeneralUtils::ObjectAsClass(UObject* Object)
{
	return Cast<UClass>(Object);
}

const UObject* UHeartGeneralUtils::GetClassDefaultObject(UClass* Class)
{
	return IsValid(Class) ? Class->GetDefaultObject() : nullptr;
}

bool UHeartGeneralUtils::IsClassDefaultObject(const UObject* Object)
{
	return Object && Object->HasAnyFlags(RF_ClassDefaultObject);
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

FVector2D UHeartGeneralUtils::ComputeSplineTangent(const FVector2D& Start, const FVector2D& End, const float Direction,
                                                   const float TensionMultiplier)
{
	auto&& RotatedStartAngle = Start.GetRotated(Direction).Y;
	auto&& RotatedEndAngle = End.GetRotated(Direction).Y;
	auto&& ReversedSpline = (RotatedStartAngle - RotatedEndAngle) < 0.f;
	auto&& TensionScalar = RotatedStartAngle - RotatedEndAngle;
	auto&& TensionVector = FVector2D(0, ReversedSpline ? -TensionMultiplier : TensionMultiplier);
	return FVector2D(TensionScalar) * TensionVector.GetRotated(Direction);
}

int32 UHeartGeneralUtils::LevenshteinDistance(const FString& A, const FString& B)
{
	return Algo::LevenshteinDistance(A, B);
}

double UHeartGeneralUtils::LevenshteinScore(const FString& A, const FString& B)
{
	if (A.IsEmpty() || B.IsEmpty())
	{
		return 0.0;
	}

	const double WorstCase = A.Len() + B.Len();
	return 1.0 - (Algo::LevenshteinDistance(A, B) / WorstCase);
}

constexpr float FontScale = 1.0f;

bool UHeartGeneralUtils::FontSupportsChar(const UFont* Font, const TCHAR Char)
{
	switch (Font->FontCacheType)
	{
	case EFontCacheType::Offline:
		{
			return Font->RemapChar(Char) != UFont::NULLCHARACTER;
		}
	case EFontCacheType::Runtime:
		{
			const TSharedPtr<FSlateFontCache> FontCache = FEngineFontServices::Get().GetFontCache();
			if (FontCache.IsValid())
			{
				return false;
			}

			const FSlateFontInfo LegacyFontInfo = Font->GetLegacySlateFontInfo();
			FCharacterList& CharacterList = FontCache->GetCharacterList(LegacyFontInfo, FontScale);
			return CharacterList.GetCharacter(Char, LegacyFontInfo.FontFallback).Valid;
		}
	default: return false;
	}
}

bool UHeartGeneralUtils::FontSupportsString(const UFont* Font, const FString& String)
{
	switch (Font->FontCacheType)
	{
	case EFontCacheType::Offline:
		{
			for (const TCHAR Char : String)
			{
				if (Font->RemapChar(Char) == UFont::NULLCHARACTER) return false;
			}
			return true;
		}
	case EFontCacheType::Runtime:
		{
			const TSharedPtr<FSlateFontCache> FontCache = FEngineFontServices::Get().GetFontCache();
			if (FontCache.IsValid())
			{
				return false;
			}

			const FSlateFontInfo LegacyFontInfo = Font->GetLegacySlateFontInfo();
			FCharacterList& CharacterList = FontCache->GetCharacterList(LegacyFontInfo, FontScale);

			for (const TCHAR Char : String)
			{
				if (!CharacterList.GetCharacter(Char, LegacyFontInfo.FontFallback).Valid) return false;
			}

			return true;
		}
	default: return false;
	}
}