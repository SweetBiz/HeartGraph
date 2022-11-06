﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartGraphActionBase.h"

bool UHeartGraphActionBase::QuickExecuteGraphAction(const TSubclassOf<UHeartGraphActionBase> Class, UObject* Target, const FHeartInputActivation& Activation)
{
	auto&& Action = CreateGraphAction(Class);
	return Action->Execute(Target, Activation);
}

UHeartGraphActionBase* UHeartGraphActionBase::CreateGraphAction(const TSubclassOf<UHeartGraphActionBase> Class)
{
	return NewObject<UHeartGraphActionBase>(GetTransientPackage(), Class);
}
