﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"

#include "HeartGraphNodeInterface.h"
#include "HeartGraphPinDesc.h"
#include "HeartGraphPinTag.h"
#include "HeartGraphPinReference.h"
#include "GraphRegistry/HeartNodeSource.h"
#include "Model/HeartGuids.h"
#include "Model/HeartPinDirection.h"

#include "HeartGraphNode.generated.h"

struct FHeartGraphNodeMessage;
class UHeartGraph;
class UHeartGraphNode;

namespace Heart
{
	class IEdNodeInterface;
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPinConnectionsChanged, const FHeartPinGuid&, Pin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGraphNodePinChanged, UHeartGraphNode*, Node);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGraphNodeLocationChanged, UHeartGraphNode*, Node, const FVector2D&, Location);

/*
UENUM(BlueprintType)
enum class EHeartNodeNameContext : uint8
{
	// Fallback when no special case needs to be considered
	Default,

	// Name shown on Graph Node instances. It is only valid to use NodeObject to determine the name under this context.
	NodeInstance,

	// Name shown in palettes or other lists
	Palette,
};
*/

UENUM(BlueprintType)
enum class EHeartPreviewNodeNameContext : uint8
{
	// Fallback when no special case needs to be considered
	Default,

	// Name shown in palettes or other lists
	Palette
};


USTRUCT(BlueprintType)
struct FHeartNodePinData
{
	GENERATED_BODY()

	void AddPin(FHeartPinGuid NewKey, const FHeartGraphPinDesc& Desc);
	bool RemovePin(FHeartPinGuid Key);

	bool Contains(FHeartPinGuid Key) const;
	int32 GetPinIndex(FHeartPinGuid Key) const;
	bool HasConnections(FHeartPinGuid Key) const;

	TOptional<FHeartGraphPinDesc> GetPinDesc(FHeartPinGuid Pin) const;

	TOptional<FHeartGraphPinConnections> GetConnections(FHeartPinGuid Key) const;
	FHeartGraphPinConnections& GetConnections(FHeartPinGuid Key);

	template <typename Predicate>
	TOptional<FHeartPinGuid> Find(Predicate Pred) const
	{
		for (auto&& Element : PinDescriptions)
		{
			if (auto Result = Pred(Element);
				Result.IsSet())
			{
				return Result.GetValue();
			}
		}
		return {};
	}

	template <typename Predicate>
	TArray<FHeartPinGuid> Filter(Predicate Pred) const
	{
		TArray<FHeartPinGuid> Out;

		for (auto&& Element : PinDescriptions)
		{
			if (Pred(Element))
			{
				Out.Add(Element.Key);
			}
		}

		return Out;
	}

	template <typename Predicate>
	void ForEach(Predicate Pred) const
	{
		for (auto&& Element : PinDescriptions)
		{
			if (!Pred(Element))
			{
				return;
			}
		}
	}

	struct FPinQueryResult
	{
		FPinQueryResult(const FHeartNodePinData& Src)
			: Reference(Src)
		{
			Src.PinOrder.GetKeys(Result);
		}

		const FHeartNodePinData& Reference;
		TArray<FHeartPinGuid> Result;

		template <typename Predicate>
		FPinQueryResult& Filter(Predicate Pred)
		{
			for (auto&& Key : Result)
			{
				if (!Pred({Key, Reference.PinDescriptions[Key]})) // @todo this is copying the description ! stop that
				{
					Result.Remove(Key);
				}
			}

			return *this;
		}

		template <typename Predicate>
		FPinQueryResult& ForEach(Predicate Pred)
		{
			for (auto&& Key : Result)
			{
				Pred(Reference.PinDescriptions[Key]);
			}

			return *this;
		}

		// Sort the results by their Pin Order
		FPinQueryResult& Sort()
		{
			Algo::SortBy(Result,
				[this](const FHeartPinGuid& Key)
				{
					return Reference.PinOrder[Key];
				});

			return *this;
		}

		// Inline sort if boolean is true
		FPinQueryResult& Sort(const bool bShouldSort)
		{
			return bShouldSort ? Sort() : *this;
		}

		template <typename Predicate>
		TOptional<FHeartPinGuid> Find(Predicate Pred) const
		{
			for (auto&& Key : Result)
			{
				if (auto Result = Pred(Reference.PinDescriptions[Key]);
					Result.IsSet())
				{
					return Result.GetValue();
				}
			}
			return {};
		}
	};

	FPinQueryResult Query() const
	{
		return FPinQueryResult(*this);
	}

protected:
	// Maps pins to their Pin Description, which carries all unique instance data about
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TMap<FHeartPinGuid, FHeartGraphPinDesc> PinDescriptions;

	// Maps pins to their connections in other nodes.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TMap<FHeartPinGuid, FHeartGraphPinConnections> PinConnections;

	// Maintains the original order of pins as added to the node.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TMap<FHeartPinGuid, int32> PinOrder;
};


/**
 * Class data for UHeartGraphNode
 */
USTRUCT()
struct FHeartGraphNodeSparseClassData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Pins")
	TArray<FHeartGraphPinDesc> DefaultPins;

	UPROPERTY(EditDefaultsOnly, Category = "Pins")
	uint8 DefaultInstancedInputs = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Pins")
	uint8 DefaultInstancedOutputs = 0;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Editor", meta = (InlineEditConditionToggle))
	bool OverrideCanCreateInEditor = false;

	// Can this node be created by the editor even if it cannot be created otherwise.
	UPROPERTY(EditDefaultsOnly, Category = "Editor", meta = (EditCondition = "OverrideCanCreateInEditor"))
	bool CanCreateInEditor = false;

	// BP properties that trigger reconstruction of SGraphNodes
	// @todo long term solution is to replace this with custom metadata on the BP properties that adds TriggersReconstruct
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	TArray<FName> PropertiesTriggeringNodeReconstruction;

	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	FName EditorSlateStyle;
#endif
};

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable, SparseClassDataTypes = "HeartGraphNodeSparseClassData")
class HEART_API UHeartGraphNode : public UObject, public IHeartGraphNodeInterface
{
	GENERATED_BODY()

	friend class UHeartGraph;
	friend class UHeartGraphSchema;
	friend class UHeartEdGraphNode;

public:
	UHeartGraphNode();

	virtual UWorld* GetWorld() const override;

	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

	/** IHeartGraphNodeInterface */
	virtual UHeartGraphNode* GetHeartGraphNode() const override final;
	/** IHeartGraphNodeInterface */


	/*----------------------------
			REFLECTION
	----------------------------*/

	FText GetDefaultNodeCategory(FHeartNodeSource NodeSource) const;
	FText GetDefaultNodeTooltip(FHeartNodeSource NodeSource) const;

	// Returns the node title used for nodes placed in a graph, which have a valid NodeObject from which to determine a title.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetNodeTitle(const UObject* Node) const;

	// Returns the node title used for template nodes, such as those in menus and palettes, that are not yet instanced.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetPreviewNodeTitle(FHeartNodeSource NodeSource, EHeartPreviewNodeNameContext Context) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetNodeCategory(const UObject* Node) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetNodeToolTip(const UObject* Node) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FLinearColor GetNodeTitleColor(const UObject* Node);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	void GetNodeMessages(TArray<FHeartGraphNodeMessage>& Messages) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FText GetInstanceTitle() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode")
	uint8 GetInstancedInputNum() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode")
	uint8 GetInstancedOutputNum() const;


	/*----------------------------
				GETTERS
	----------------------------*/

	template <typename TNodeClass>
	TNodeClass* GetNodeObject() const
	{
		return Cast<TNodeClass>(NodeObject);
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UObject* GetNodeObject() const { return NodeObject; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeprecatedFunction, DeterminesOutputType = Class))
	UObject* GetNodeObjectTyped(TSubclassOf<UObject> Class) const { return NodeObject; }

	template <typename THeartGraph>
	THeartGraph* GetOwningGraph() const
	{
		static_assert(TIsDerivedFrom<THeartGraph, UHeartGraph>::IsDerived, "The graph class must derive from UHeartGraph");
		return Cast<THeartGraph>(GetGraph());
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraph* GetGraph() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = "Class", DeprecatedFunction))
	UHeartGraph* GetGraphTyped(TSubclassOf<UHeartGraph> Class) const { return GetGraph(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FHeartNodeGuid GetGuid() const { return Guid; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FVector2D GetLocation() const { return Location; }

	// @todo enable UFUNCTION in 5.4
	//UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	TOptional<FHeartGraphPinDesc> GetPinDesc(const FHeartPinGuid& Pin) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FHeartGraphPinDesc GetPinDescChecked(const FHeartPinGuid& Pin) const;

	UE_DEPRECATED(5.3, "Replace with either GetPinDescChecked or (after 5.4) GetPinDesc")
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FHeartGraphPinDesc GetPinDesc(const FHeartPinGuid& Pin, bool DeprecationTemp) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FHeartGraphPinReference GetPinReference(const FHeartPinGuid& Pin) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (AutoCreateRefTerm = "Name"))
	FHeartPinGuid GetPinByName(const FName& Name) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode")
	TArray<FHeartPinGuid> GetPinsOfDirection(EHeartPinDirection Direction, bool bSorted = false) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode")
	TArray<FHeartPinGuid> GetInputPins(bool bSorted = false) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode")
	TArray<FHeartPinGuid> GetOutputPins(bool bSorted = false) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Heart|GraphNode")
	TArray<FHeartGraphPinDesc> GetDynamicPins() const;

	// Declare the pin typed used for instanced pins. Overriding this is required for User Input/Output to work.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure = false, Category = "Heart|GraphNode")
	void GetInstancedPinData(EHeartPinDirection Direction, FHeartGraphPinTag& Tag, TArray<UHeartGraphPinMetadata*>& Metadata) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	bool HasConnections(const FHeartPinGuid& Pin) const;

	// @todo this function is a good candidate to move to a library
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	TSet<UHeartGraphNode*> GetConnectedGraphNodes(EHeartPinDirection Direction = EHeartPinDirection::Bidirectional) const;


	/*----------------------------
			NODE EDITING
	----------------------------*/
public:
#if WITH_EDITOR
	// @todo move this to EdGraphNode class
	virtual bool CanCreate_Editor() const;
#endif

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	void SetLocation(const FVector2D& NewLocation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanUserAddInput() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanUserAddOutput() const;

	// Can the user create instances of this node. Only necessary to override for use in graphs with the ability to spawn nodes.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanCreate() const;

	// Can the user delete this node. Only necessary to override for use in graphs with the ability to delete nodes.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanDelete() const;

	// Can the user duplicate this node. Only necessary to override for use in graphs with the ability to duplicate nodes.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanDuplicate() const;


	/*----------------------------
			PIN EDITING
	----------------------------*/

	FHeartGraphPinConnections& GetLinks(const FHeartPinGuid& Pin);

	TOptional<FHeartGraphPinConnections> GetLinks(const FHeartPinGuid& Pin) const;

	UE_DEPRECATED(5.4, "Use a different version of GetLinks please")
	FHeartGraphPinConnections GetLinks(const FHeartPinGuid& Pin, bool DeprecationTemp) const;

	FHeartNodePinData::FPinQueryResult QueryPins() const { return PinData.Query(); }

	// Get all pins that match the direction.
	FHeartNodePinData::FPinQueryResult FindPinsByDirection(EHeartPinDirection Direction) const;

	// Get all pins that match the direction and custom predicate.
	template <typename Predicate>
	FHeartNodePinData::FPinQueryResult FindPinsByPredicate(EHeartPinDirection Direction, Predicate Pred) const;

	// Remove all pins that match the predicate. Returns num of pins removed.
	template <typename Predicate>
	int32 RemovePinsByPredicate(EHeartPinDirection Direction, Predicate Pred);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FHeartPinGuid AddPin(const FHeartGraphPinDesc& Desc);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	bool RemovePin(const FHeartPinGuid& Pin);

	// Add a numbered instance pin
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FHeartPinGuid AddInstancePin(EHeartPinDirection Direction);

	// Remove the last numbered instance pin
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	void RemoveInstancePin(EHeartPinDirection Direction);

	virtual void NotifyPinConnectionsChanged(const FHeartPinGuid& Pin);

protected:
	// Called by the owning graph when we are created.
	virtual void OnCreate();

	void ReconstructPins();

	// Called by the owning graph when we are created.
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|GraphNode", DisplayName = "On Create")
	void BP_OnCreate();

	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|GraphNode", DisplayName = "On Connections Changed")
	void BP_OnConnectionsChanged(FHeartPinGuid Pin);

public:
	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FOnPinConnectionsChanged OnPinConnectionsChanged;

	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FOnGraphNodePinChanged OnNodePinsChanged;

	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FOnGraphNodeLocationChanged OnNodeLocationChanged;

protected:
	// The object that this graph node represents. Contains the data and functionality of a spawned instance.
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UObject> NodeObject;

	UPROPERTY()
	FHeartNodeGuid Guid;

	UPROPERTY()
	FVector2D Location;

	UE_DEPRECATED(5.3, "Use PinData instead")
	UPROPERTY()
	TMap<FHeartPinGuid, FHeartGraphPinDesc> PinDescriptions;

	UE_DEPRECATED(5.3, "Use PinData instead")
	UPROPERTY()
	TMap<FHeartPinGuid, FHeartGraphPinConnections> PinConnections;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Heart|GraphNode")
	FHeartNodePinData PinData;

	UPROPERTY(BlueprintReadOnly, Category = "Heart|GraphNode")
	uint8 InstancedInputs = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Heart|GraphNode")
	uint8 InstancedOutputs = 0;

#if WITH_EDITOR
private:
	Heart::IEdNodeInterface* EdGraphNodePointer;
#endif
};


/*----------------------------
		TEMPLATE IMPL.
----------------------------*/

template <typename Predicate>
FHeartNodePinData::FPinQueryResult UHeartGraphNode::FindPinsByPredicate(const EHeartPinDirection Direction, Predicate Pred) const
{
	return PinData.Query()
		.Filter([Direction](const TTuple<FHeartPinGuid, FHeartGraphPinDesc>& PinPair)
			{
				return EnumHasAnyFlags(Direction, PinPair.Value.Direction);
			})
		.Filter(Pred);
}

template <typename Predicate>
int32 UHeartGraphNode::RemovePinsByPredicate(const EHeartPinDirection Direction, Predicate Pred)
{
	TArray<FHeartPinGuid> PinsToRemove = FindPinsByPredicate(Direction, Pred).Result;

	for (auto&& ToRemove : PinsToRemove)
	{
		RemovePin(ToRemove);
	}

	return PinsToRemove.Num();
}