#include "PGInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DataTable.h"

UPGInventoryComponent::UPGInventoryComponent()
{
    SetIsReplicatedByDefault(true);

    ItemDataTable = TSoftObjectPtr<UDataTable>(
        FSoftObjectPath(TEXT("/Game/Paragonia/Data/DT_Item.DT_Item"))
    );
}

UDataTable* UPGInventoryComponent::GetItemDataTable() const
{
    return ItemDataTable.IsNull() ? nullptr : ItemDataTable.LoadSynchronous();
}

void UPGInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    if (GetOwner()->HasAuthority())
    {
        Slots.SetNum(MaxSlots);
        for (auto& S : Slots)
        {
            S = FInventorySlot(); 
        }
        OnInventoryChanged.Broadcast();
    }
}

void UPGInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UPGInventoryComponent, Slots);
}

const FInventorySlot& UPGInventoryComponent::GetSlot(int32 Index) const
{
    check(Slots.IsValidIndex(Index));
    return Slots[Index];
}

bool UPGInventoryComponent::AddItem(FName ItemId)
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        ServerAddItem(ItemId);
        return true;
    }

    for (FInventorySlot& Slot : Slots)
    {
        if (Slot.bEmpty)
        {
            Slot.bEmpty = false;
            Slot.ItemId = ItemId;
            Slot.Count = 1;

            OnInventoryChanged.Broadcast();
            return true;
        }
    }

    return false; 
}

void UPGInventoryComponent::RequestSwapSlot(int32 FromIndex, int32 ToIndex)
{
    if (!Slots.IsValidIndex(FromIndex) || !Slots.IsValidIndex(ToIndex))
    {
        return;
    }

    if (GetOwnerRole() < ROLE_Authority)
    {
        ServerSwapSlot(FromIndex, ToIndex);
        return;
    }

    Slots.Swap(FromIndex, ToIndex);
    OnInventoryChanged.Broadcast();
}

void UPGInventoryComponent::ServerSwapSlot_Implementation(int32 FromIndex, int32 ToIndex)
{
    if (!Slots.IsValidIndex(FromIndex) || !Slots.IsValidIndex(ToIndex))
    {
        return;
    }

    Slots.Swap(FromIndex, ToIndex);
    OnInventoryChanged.Broadcast();
}

void UPGInventoryComponent::ServerAddItem_Implementation(FName ItemId)
{
    AddItem(ItemId);
}

void UPGInventoryComponent::OnRep_Inventory()
{
    OnInventoryChanged.Broadcast();
}
