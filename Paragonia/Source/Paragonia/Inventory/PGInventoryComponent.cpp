#include "PGInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DataTable.h"
#include "AbilitySystemComponent.h"
#include "Shop/PGShopTypes.h" 
#include "AbilitySystemInterface.h"
#include "Character/PGPlayerCharacterBase.h"
#include "PlayerState/PGPlayerState.h"

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

FActiveGameplayEffectHandle UPGInventoryComponent::ApplyAllItemStatsToOwner(const FPGShopItemRow& Item)
{
    UDataTable* DT = GetItemDataTable();
    if (!DT)
    {
        return FActiveGameplayEffectHandle();
    }

    FPGShopItemRow* ItemRow = DT->FindRow<FPGShopItemRow>(Item.ItemId, TEXT(""));
    if (!ItemRow || !ItemRow->EquipmentGE)
    {
        return FActiveGameplayEffectHandle();
    }

    APGPlayerState* PS = Cast<APGPlayerState>(GetOwner());
    if (!PS)
    {
        return FActiveGameplayEffectHandle();
    }

    APGPlayerCharacterBase* OwningPawn = Cast<APGPlayerCharacterBase>(PS->GetPawn());
    if (!OwningPawn)
    {
        return FActiveGameplayEffectHandle();
    }

    IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwningPawn);
    UAbilitySystemComponent* ASC = nullptr;

    if (ASI)
    {
        ASC = ASI->GetAbilitySystemComponent();
    }
    else
    {
        ASC = OwningPawn->FindComponentByClass<UAbilitySystemComponent>();
    }

    if (ASC)
    {
        FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
        FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ItemRow->EquipmentGE, 1.0f, Context);

        if (SpecHandle.IsValid())
        {
            for (const auto& StatPair : ItemRow->ItemStats)
            {
                SpecHandle.Data.Get()->SetSetByCallerMagnitude(StatPair.Key, StatPair.Value);
            }

            return ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }

    return FActiveGameplayEffectHandle();
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

bool UPGInventoryComponent::HasEmptySlot() const
{
    for (const FInventorySlot& Slot : Slots)
    {
        if (Slot.bEmpty)
        {
            return true;
        }
    }

    return false;
}

bool UPGInventoryComponent::AddItem(FName ItemId)
{
    // 지급은 서버에서 ShopComponent를 거쳐서만 일어난다. 클라이언트 요청 경로는 두지 않는다
    if (GetOwnerRole() < ROLE_Authority)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Inventory] AddItem is server only"));
        return false;
    }

    for (FInventorySlot& Slot : Slots)
    {
        if (Slot.bEmpty)
        {
            Slot.bEmpty = false;
            Slot.ItemId = ItemId;
            Slot.Count = 1;

            Slot.AppliedEffectHandle = ApplyItemStatsToOwner(ItemId);

            OnInventoryChanged.Broadcast();
            return true;
        }
    }

    return false; 
}

bool UPGInventoryComponent::RemoveItem(int32 Index)
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Inventory] RemoveItem is server only"));
        return false;
    }

    if (!Slots.IsValidIndex(Index))
    {
        return false;
    }

    if (!Slots[Index].bEmpty)
    {
        if (Slots[Index].AppliedEffectHandle.IsValid())
        {
            APGPlayerState* PS = Cast<APGPlayerState>(GetOwner());
            if (PS && PS->GetPawn())
            {
                UAbilitySystemComponent* ASC = PS->GetPawn()->FindComponentByClass<UAbilitySystemComponent>();
                if (ASC)
                {
                    ASC->RemoveActiveGameplayEffect(Slots[Index].AppliedEffectHandle);
                }
            }
        }

        if(Slots[Index].Count > 1)
        {
            Slots[Index].Count -= 1;
		}
        else {
            Slots[Index] = FInventorySlot();
        }

        OnInventoryChanged.Broadcast();
        return true;
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

void UPGInventoryComponent::OnRep_Inventory()
{
    OnInventoryChanged.Broadcast();
}

FActiveGameplayEffectHandle UPGInventoryComponent::ApplyItemStatsToOwner(FName ItemId)
{
    UDataTable* DT = GetItemDataTable();
    if (!DT) 
    {
        return FActiveGameplayEffectHandle();
    }

    FPGShopItemRow* ItemRow = DT->FindRow<FPGShopItemRow>(ItemId, TEXT(""));
    if (!ItemRow || !ItemRow->EquipmentGE) 
    {
        return FActiveGameplayEffectHandle();
    }

    APGPlayerState* PS = Cast<APGPlayerState>(GetOwner());
    if (!PS) 
    {
        return FActiveGameplayEffectHandle();
    }

    APGPlayerCharacterBase* OwningPawn = Cast<APGPlayerCharacterBase>(PS->GetPawn());
    if (!OwningPawn)
    {
        return FActiveGameplayEffectHandle();
    }

    IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwningPawn);
    UAbilitySystemComponent* ASC = nullptr;

    if (ASI)
    {
        ASC = ASI->GetAbilitySystemComponent();
    }
    else
    {
        ASC = OwningPawn->FindComponentByClass<UAbilitySystemComponent>();
    }

    if (ASC)
    {
        FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
        FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ItemRow->EquipmentGE, 1.0f, Context);

        if (SpecHandle.IsValid())
        {
            for (const auto& StatPair : ItemRow->ItemStats)
            {
                SpecHandle.Data.Get()->SetSetByCallerMagnitude(StatPair.Key, StatPair.Value);
            }

            return ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }

    return FActiveGameplayEffectHandle();
}