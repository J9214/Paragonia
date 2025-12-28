#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "PG_IngameHUD.generated.h"

class UPG_HPBar;
class UPG_InGameTeamSimpleInfo;
class UPG_MiniMap;
class UPG_SkillIcon;
class UTextureRenderTarget2D;
class UCharacterAttributeSet;

UCLASS()
class PARAGONIA_API UPG_IngameHUD : public UCommonUserWidget
{
	GENERATED_BODY()

public:

	void BindToPlayerAttributeSet(UCharacterAttributeSet* InAttrSet);
	void BindToTeam1AttributeSet(UCharacterAttributeSet* InAttrSet);
	void BindToTeam2AttributeSet(UCharacterAttributeSet* InAttrSet);

	UFUNCTION(BlueprintCallable)
	void InitMinimap(UTextureRenderTarget2D* InRT);

	UFUNCTION()
	void InitTeam1IngameIcon(int32 CharacterID);

	UFUNCTION()
	void InitTeam2IngameIcon(int32 CharacterID);

	UFUNCTION()
	void HandlePlayerHealthChanged(float OldValue, float NewValue);

	UFUNCTION()
	void HandlePlayerMaxHealthChanged(float OldValue, float NewValue);

	UFUNCTION()
	void HandleTeam1HealthChanged(float OldValue, float NewValue);

	UFUNCTION()
	void HandleTeam1MaxHealthChanged(float OldValue, float NewValue);

	UFUNCTION()
	void HandleTeam2HealthChanged(float OldValue, float NewValue);

	UFUNCTION()
	void HandleTeam2MaxHealthChanged(float OldValue, float NewValue);

	UFUNCTION()
	void HandleCooldownTimeChanged(FGameplayTag CooldownTag, float Remaining, float Duration);

	UFUNCTION()
	void HandleCooldownTagChanged(FGameplayTag CooldownTag, int32 NewCount);

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeDestruct() override;
private:
	void BindCooldownToSkillIcon();

	void UnbindFromPlayerAttributeSet();
	void UnbindFromTeam1AttributeSet();
	void UnbindFromTeam2AttributeSet();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPG_MiniMap> MiniMapWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPG_HPBar> PlayerHPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPG_InGameTeamSimpleInfo> Team1HPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPG_InGameTeamSimpleInfo> Team2HPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPG_SkillIcon> SkillIconQ;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPG_SkillIcon> SkillIconE;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPG_SkillIcon> SkillIconR;

private:
	TMap<FGameplayTag, TObjectPtr<UPG_SkillIcon>> CooldownTagToWidget;

	UPROPERTY()
	TObjectPtr<UCharacterAttributeSet> BoundPlayerAttrSet;
	UPROPERTY()
	TObjectPtr<UCharacterAttributeSet> BoundTeam1AttrSet;
	UPROPERTY()
	TObjectPtr<UCharacterAttributeSet> BoundTeam2AttrSet;
};
