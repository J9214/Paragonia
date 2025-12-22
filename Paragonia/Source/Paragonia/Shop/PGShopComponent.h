// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "PGShopTypes.h"
#include "PGShopComponent.generated.h"


class UPGShopCatalog;

UENUM(BlueprintType)
enum class EShopBuyResult : uint8
{
    Success,
    NotEnoughGold,
    OutOfStock,
    InvalidItem
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnBuyResult,
    EShopBuyResult, Result,
    FName, ItemId
);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PARAGONIA_API UPGShopComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPGShopComponent();

    UFUNCTION(BlueprintCallable, Category = "Shop|Data")
    UDataTable* GetItemDataTable() const;

    UFUNCTION(BlueprintCallable)
    bool FindItem(FName ItemId, FPGShopItemRow& OutItem) const;

    UFUNCTION(BlueprintCallable)
    int32 GetRemainingStock(FName ItemId) const;

    UFUNCTION(BlueprintCallable)
    void RequestBuy(FName ItemId);

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(Server, Reliable)
    void ServerBuy(FName ItemId);

    UFUNCTION(Client, Reliable)
    void ClientBuyResult(EShopBuyResult Result, FName ItemId);

    void GrantItemToOwner(const FPGShopItemRow& Item);

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Data")
    TSoftObjectPtr<UDataTable> ItemDataTable;

    UPROPERTY(BlueprintAssignable)
    FOnBuyResult OnBuyResult;
};