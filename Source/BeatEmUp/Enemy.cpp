// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "BrainComponent.h"
#include "EnemyBTController.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy() {
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	Player = nullptr;

	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	HealthBarWidgetComponent->SetDrawSize(FVector2D(200, 50));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay() {
	Super::BeginPlay();

	Player = Cast<ABeatEmUpCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	GetCharacterMovement()->bOrientRotationToMovement = true;  // Always rotates to face the direction that it's moving in.
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);

	if(EnemyStatusWidgetClass) {
		HealthBarWidgetComponent->SetWidgetClass(EnemyStatusWidgetClass);
		HealthBarWidgetComponent->SetRelativeLocation(FVector(0, 0, 105));
	}
	
	HealthBar = Cast<UEnemyStatusWidget>(HealthBarWidgetComponent->GetUserWidgetObject());
	if(HealthBar) {
		HealthBar->Enemy = this;
		HealthBar->UpdateValues();
		HealthBar->HideWarningIcon();
		HealthBar->HideInvestigateIcon();
	}
}

void AEnemy::UpdateWidgetRotation() const {
	if(!HealthBarWidgetComponent) return;
	if(!GetWorld()->GetFirstPlayerController()) return;
	if(!GetWorld()->GetFirstPlayerController()->PlayerCameraManager) return;
	
	const FVector CameraLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
	const FVector Direction = CameraLocation - HealthBarWidgetComponent->GetComponentLocation();
	const FRotator NewRot = FRotationMatrix::MakeFromX(Direction).Rotator();
	HealthBarWidgetComponent->SetWorldRotation(NewRot);
}

void AEnemy::Shoot(FVector Direction) {
	const FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100;
	const FRotator SpawnRotation = Direction.Rotation();
	const ABullet* SpawnedBullet = Cast<ABullet>(GetWorld()->SpawnActor(BulletClass, &SpawnLocation, &SpawnRotation));
	Direction.Normalize();
	SpawnedBullet->MovementComponent->Velocity = Direction * SpawnedBullet->MovementSpeed;
}

// Called every frame
void AEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	UpdateWidgetRotation();
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::DealDamage(float Damage) {
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	if(HealthBar) HealthBar->UpdateValues();

	UDamageTextUI* SpawnedDamage = Cast<UDamageTextUI>(CreateWidget(GetGameInstance(), DamageTextClass));
	UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), GetMesh()->GetComponentLocation(), SpawnedDamage->CurrentLocation);
	SpawnedDamage->DamageText->SetText(FText::FromString(FString::SanitizeFloat(Damage)));
	SpawnedDamage->TargetLocation += SpawnedDamage->CurrentLocation;
	SpawnedDamage->AddToViewport();
}

void AEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector Normal,
	const FHitResult& Hit) {
	if(OtherActor != Player) return;
	
	Player->DealDamage(DamageToPlayer);
	Destroy();
}

void AEnemy::Ragdoll() {
	Cast<AEnemyBTController>(GetController())->BrainComponent->PauseLogic("Rag dolling!");

	HealthBar->SetVisibility(ESlateVisibility::Hidden);
	
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
	GetWorld()->GetTimerManager().SetTimer(RagdollTimerHandle, this, &AEnemy::StopRagdoll, RagdollTime, false);
}

void AEnemy::StopRagdoll() {
	Cast<AEnemyBTController>(GetController())->BrainComponent->ResumeLogic("Moving Again!");
	
	if(CurrentHealth <= 0) {
		Player->AddExp(ExpAmount);
		if(DeathParticleClass) {
			UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DeathParticleClass, GetMesh()->GetComponentLocation());
			SpawnedEffect->SetColorParameter(FName("User.SpawnColour"), FLinearColor::MakeRandomColor());
		}
		if(HealthPackClass) {
			const FVector SpawnLocation = GetMesh()->GetComponentLocation() + FVector(0, 0, 50);
			GetWorld()->SpawnActor(HealthPackClass, &SpawnLocation);
		}
		Destroy();
		return;
	}
	
	HealthBar->SetVisibility(ESlateVisibility::Visible);

	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionProfileName("CharacterMesh");
	GetCapsuleComponent()->SetWorldLocation(GetMesh()->GetSocketLocation("pelvis"));
	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	GetCapsuleComponent()->SetCollisionProfileName("Pawn");
	GetCharacterMovement()->GravityScale = 1;
}
