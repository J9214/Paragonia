#include "Bullet/PGTaskRelatedBullet.h"

APGTaskRelatedBullet::APGTaskRelatedBullet()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APGTaskRelatedBullet::InitBullet(const FAttackData& InAttackData)
{
	AttackData = InAttackData;
}
