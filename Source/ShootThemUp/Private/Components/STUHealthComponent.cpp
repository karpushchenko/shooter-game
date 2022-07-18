// Shoot Them Up Game


#include "Components/STUHealthComponent.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All);

USTUHealthComponent::USTUHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USTUHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	SetHealth(MaxHealth);

    AActor* ComponentOwner = GetOwner();
    if ( ComponentOwner )
    {
        ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &USTUHealthComponent::OnTakeAnyDamage);
    }
}

void USTUHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy,
    AActor* DamageCauser)
{
    if ( Damage <= 0.0f || IsDead() || !GetWorld() ) return;
    
    SetHealth(Health - Damage);

    if (IsDead())
    {
        OnDeath.Broadcast();
    } else if( AllowAutoHeal )
    {
        GetWorld()->GetTimerManager().SetTimer(HealTimerHandle, this, &USTUHealthComponent::AutoHeal, HealDelay, false);
    }
}

void USTUHealthComponent::AutoHeal()
{
    if(GetHealth() < MaxHealth && !IsDead())
    {
        GetWorld()->GetTimerManager().SetTimer(HealTimerHandle, this, &USTUHealthComponent::AutoHeal, HealUpdateTime, false);
        SetHealth(Health + HealModifier);
    } else
    {
        GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
    }
}

void USTUHealthComponent::SetHealth(float NewHealth)
{
    Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(Health);
}
