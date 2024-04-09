﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartInputHandlerAssetBase.h"
#include "HeartInputTypes.h"
#include "HeartInputHandler_Immediate.generated.h"

/**
 * Base class for simple handlers
 */
UCLASS(Abstract)
class HEARTCORE_API UHeartInputHandler_Immediate : public UHeartInputHandlerAssetBase
{
	GENERATED_BODY()

public:
	virtual Heart::Input::EExecutionOrder GetExecutionOrder() const override
	{
		return HandleInput ? Heart::Input::Event : Heart::Input::Listener;
	}

protected:
	// Does this handler block input from bubbling (a capture), or allow other handlers to also respond to it.
	UPROPERTY(EditAnywhere, Category = "Trigger")
	bool HandleInput = true;
};