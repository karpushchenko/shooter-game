// Shoot Them Up Game

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUHealthComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTTHEMUP_API USTUHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USTUHealthComponent();

    float GetHealth() const { return Health; }

    UFUNCTION(BlueprintCallable)
    bool IsDead() const
    {
        return FMath::IsNearlyZero(Health);
    }

    FOnDeathSignature OnDeath;
    FOnHealthChangedSignature OnHealthChanged;

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Health", meta = (ClampMin = "0.0", ClampMax = "1000.0"))
    float MaxHealth = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Heal")
    bool AllowAutoHeal = true;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Heal", meta = (EditCondition = "AllowAutoHeal"))
    float HealUpdateTime = 0.3f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Heal", meta = (EditCondition = "AllowAutoHeal"))
    float HealDelay = 3.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Heal", meta = (EditCondition = "AllowAutoHeal"))
    float HealModifier = 1.0f;
    
	virtual void BeginPlay() override;

    private:
        float Health = 0.0f;
        FTimerHandle HealTimerHandle;

        UFUNCTION()
        void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
    
        void AutoHeal();
        void SetHealth(float NewHealth);
		
};
