// Shoot Them Up Game


#include "Player/STUBaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/STUCharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/STUHealthComponent.h"
#include "Components/TextRenderComponent.h"

DEFINE_LOG_CATEGORY_STATIC(BaseCharacterLog, All, All);

// Sets default values
ASTUBaseCharacter::ASTUBaseCharacter( const FObjectInitializer& ObjInit ):Super(ObjInit.SetDefaultSubobjectClass<USTUCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->bUsePawnControlRotation = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    CameraComponent->SetupAttachment(SpringArmComponent);

    HealthComponent = CreateDefaultSubobject<USTUHealthComponent>("HealthComponent");
    
    HealthTextComponent = CreateDefaultSubobject<UTextRenderComponent>("HealthTextComponent");
    HealthTextComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ASTUBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

    // Dev checks
    check(HealthComponent);
    check(HealthTextComponent);
}

// Called every frame
void ASTUBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    const auto Health = HealthComponent->GetHealth();
    HealthTextComponent->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Health)));

    TakeDamage( 0.1f, FDamageEvent{}, Controller, this );
}

// Called to bind functionality to input
void ASTUBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ASTUBaseCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ASTUBaseCharacter::MoveRight);
    PlayerInputComponent->BindAxis("LookUp", this, &ASTUBaseCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("TurnAround", this, &ASTUBaseCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASTUBaseCharacter::Jump);
    PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASTUBaseCharacter::OnStopRunning);
    PlayerInputComponent->BindAction("Run", IE_Released, this, &ASTUBaseCharacter::OnStartRunning);
}

bool ASTUBaseCharacter::IsRunning() const
{
    return WantsToRun && IsMovingForward && !GetVelocity().IsZero();
}

float ASTUBaseCharacter::GetMovementDirection() const
{
    if(GetVelocity().IsZero()) return 0.0f;
    
    // Getting Normal
    const auto VelocityNormal = GetVelocity().GetSafeNormal();
    // Getting Arc cos from Dot product between forward vector and normal vector
    const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), VelocityNormal));
    // Orthogonal vector
    const auto CrossProduct = FVector::CrossProduct(GetActorForwardVector(), VelocityNormal);

    const auto Degrees = FMath::RadiansToDegrees(AngleBetween);
    
    // Convert to degrees and multiply to a cross product sign
    return CrossProduct.IsZero() ? Degrees : Degrees * FMath::Sign(CrossProduct.Z);
}

void ASTUBaseCharacter::MoveForward(float Amount)
{
    IsMovingForward = Amount > 0.0f;
    if( Amount == 0.0f ) return;
    AddMovementInput(GetActorForwardVector(), Amount);
}

void ASTUBaseCharacter::MoveRight(float Amount)
{
    if( Amount == 0.0f ) return;
    AddMovementInput(GetActorRightVector(), Amount);
}

void ASTUBaseCharacter::OnStartRunning()
{
    WantsToRun = false;
}

void ASTUBaseCharacter::OnStopRunning()
{
    WantsToRun = true;
}
