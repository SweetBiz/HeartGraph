﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "ModelView/Actions/HeartGraphActionBase.h"
#include "HeartGraphCanvasAction.generated.h"

class UHeartGraphCanvas;
class UHeartGraphCanvasNode;
class UHeartGraphCanvasPin;

/**
 *
 */
UCLASS(Abstract, BlueprintType)
class HEARTCANVAS_API UHeartGraphCanvasAction : public UHeartGraphActionBase
{
	GENERATED_BODY()

public:
	virtual bool Execute(UObject* Object, const FHeartInputActivation& Activation, UObject* ContextObject) override;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphAction")
	virtual void ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation, UObject* ContextObject) {}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphAction")
	virtual void ExecuteOnNode(UHeartGraphCanvasNode* CanvasNode, const FHeartInputActivation& Activation, UObject* ContextObject) {}

	UFUNCTION(BlueprintCallable, Category = "Heart|PinAction")
	virtual void ExecuteOnPin(UHeartGraphCanvasPin* CanvasPin, const FHeartInputActivation& Activation, UObject* ContextObject) {}
};

// @todo blueprintbase should be hyphenated
UCLASS(Abstract, Blueprintable)
class HEARTCANVAS_API UHeartGraphCanvasActionBlueprintBase : public UHeartGraphCanvasAction
{
	GENERATED_BODY()

public:
	virtual void ExecuteOnGraph(UHeartGraphCanvas* Graph, const FHeartInputActivation& Activation, UObject* ContextObject) override final;
	virtual void ExecuteOnNode(UHeartGraphCanvasNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject) override final;
	virtual void ExecuteOnPin(UHeartGraphCanvasPin* Pin, const FHeartInputActivation& Activation, UObject* ContextObject) override final;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Graph"))
	void BP_ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation, UObject* ContextObject);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Node"))
	void BP_ExecuteOnNode(UHeartGraphCanvasNode* CanvasNode, const FHeartInputActivation& Activation, UObject* ContextObject);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Execute on Pin"))
	void BP_ExecuteOnPin(UHeartGraphCanvasPin* CanvasPin, const FHeartInputActivation& Activation, UObject* ContextObject);
};