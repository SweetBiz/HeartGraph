﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphAction.h"
#include "HeartPinAction_Disconnect.generated.h"

/**
 *
 */
UCLASS()
class HEART_API UHeartPinAction_Disconnect : public UHeartGraphAction
{
	GENERATED_BODY()

public:
	virtual void ExecuteOnPin(UHeartGraphPin* Pin, const FHeartInputActivation& Activation) override;
};
