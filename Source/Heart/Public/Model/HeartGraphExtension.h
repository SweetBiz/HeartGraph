﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGraphExtension.generated.h"

class UHeartGraph;

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, Within = HeartGraph)
class HEART_API UHeartGraphExtension : public UObject
{
	GENERATED_BODY()

	friend UHeartGraph;

public:
	// Get the owning Heart Graph
	UFUNCTION(BlueprintCallable, Category = "Heart|Extension")
	UHeartGraph* GetGraph() const;

protected:
	virtual void PostExtensionAdded() {}
	virtual void PreExtensionRemove() {}
};