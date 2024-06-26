﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartCanvasDragDropOperation.h"
#include "HeartCanvasDragViewDragDropOperation.generated.h"

/**
 *
 */
UCLASS()
class HEARTCANVAS_API UHeartCanvasDragViewDragDropOperation : public UHeartCanvasDragDropOperation
{
	GENERATED_BODY()

public:
	virtual bool CanRunOnWidget(const UWidget* Widget) const override;

	virtual bool SetupDragDropOperation() override;

	virtual void Dragged_Implementation(const FPointerEvent& PointerEvent) override;

private:
	TWeakObjectPtr<class UHeartGraphCanvas> Canvas;

	// Mouse position last frame
	FVector2f DeltaMousePosition = FVector2f::ZeroVector;
};