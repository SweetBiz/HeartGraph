﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Input/HeartInputLinkerInterface.h"
#include "Input/HeartWidgetInputLinker.h"
#include "HeartGraphWidgetBase.generated.h"

/**
 *
 */
UCLASS(Abstract)
class HEARTCANVAS_API UHeartGraphWidgetBase : public UUserWidget, public IHeartInputLinkerInterface
{
	GENERATED_BODY()

public:
	UHeartGraphWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	HEART_UMG_INPUT_LINKER_HEADER()

	/** UWidget */
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
	/** UWidget */

	// Unless overriden with custom behavior, this will walk up the widget tree looking for something that implements this
	virtual UHeartInputLinkerBase* ResolveLinker_Implementation() const override;
};