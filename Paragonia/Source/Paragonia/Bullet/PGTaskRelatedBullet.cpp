#include "Bullet/PGTaskRelatedBullet.h"

APGTaskRelatedBullet::APGTaskRelatedBullet()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APGTaskRelatedBullet::InitBullet(UGameplayAbility* InAbility, UAbilityTask_WaitTargetData* InTask, const FAttackData& InAttackData)
{
	Ability = InAbility;
	Task = InTask;
	AttackData = InAttackData;
}
