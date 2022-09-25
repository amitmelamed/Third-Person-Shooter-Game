// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Item.h"
#include "Components/WidgetComponent.h"
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Ammo.h"



// Sets default values
AShooterCharacter::AShooterCharacter() :
	//Base rate for turning/looking up
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	// Turn rates for aiming/not aiming
	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AimingTurnRate(20.f),
	AimingLookUpRate(20.f),
	bAiming(false),
	//Camera FOV for aiming/not aiming valus
	CameraDefaultFOV(0.f),//Changing this in begin play function
	CameraZoomedFOV(35.f),
	CrouchingCameraZommedFOV(45.f),
	ZoomInterpSpeed(20.f),
	CameraCurrentFOV(0.f),
	//Camera speed while aiming/now aiming difference while using mouse
	AimingSensetivityMultiplier(0.8f),
	//Bullet Fire timer variables
	ShootTimeDuration(0.15f),
	bFiringBullet(false),
	//Automatic fire vairables
	AutomaticFireRate(0.15f),
	bShouldFire(true),
	bFireBottonPressed(false),
	bShouldTraceForItems(false),
	CameraInterpDistance(250.f),
	CameraInterpElevation(65.f),
	Starting9mmAmmo(85),
	StartingARAmmo(120),
	StartingShotgunAmmo(40),
	CombatState(ECombatState::ECS_Unoccupied),
	bCrouching(false),
	StandingCapsuleHalfHeight(88.f),
	CrouchingCapsuleHalfHeight(44.f),
	BaseGroundFriction(2.f),
	CrouchingGroundFriction(100.f),
	//Pickup sound timer properties
	bShouldPlayEquipSound(true),
	bShouldPlayPickupSound(true),
	PickupSoundResetTime(0.2f),
	EquipSoundResetTime(0.2f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create Camera Boom (pulls in towords character if there is collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f; //The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; //Rorate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	//Create a follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); //Attach camera to end of boom
	FollowCamera->bUsePawnControlRotation = false; //Camera doesnt rotate relative to arm

	//Dont rotate player with camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//Configure Character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; //Character move in the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.f,540.f,0.f); //... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//Create Hand Scene Component
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComponent"));

	//Create Interpolation Components
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component"));
	WeaponInterpComp->SetupAttachment(GetFollowCamera());

	InterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 1"));
	InterpComp1->SetupAttachment(GetFollowCamera());

	InterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 2"));
	InterpComp2->SetupAttachment(GetFollowCamera());

	InterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 3"));
	InterpComp3->SetupAttachment(GetFollowCamera());

	InterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 4"));
	InterpComp4->SetupAttachment(GetFollowCamera());

	InterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 5"));
	InterpComp5->SetupAttachment(GetFollowCamera());

	InterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 6"));
	InterpComp6->SetupAttachment(GetFollowCamera());
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (FollowCamera) 
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	//Spawn default weapon and equip it
	EquipWeapon(SpawnDefaultWeapon());
	
	if (EquippedWeapon) 
	{
		EquippedWeapon->DisableCustomDepth();
		EquippedWeapon->DisableGlowMaterial();
	}
	
	InitializeAmmoMap();

	//Create FInterpLocation structs for each interp location, Add them to array
	InitializeInterpLocations();
	
}

void AShooterCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0) {
		//Find out which way is forward
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw,0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0) {
		//Find out which way is forward
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw,0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	//Calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); //  deg/sec * sec/frame 
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()); //  deg/sec * sec/frame 
}

void AShooterCharacter::Turn(float Value)
{
	if (bAiming) {
		Value = Value * AimingSensetivityMultiplier;
	}
	if (Value != 0.f && Controller && Controller->IsLocalPlayerController())
	{
		APlayerController* const PC = CastChecked<APlayerController>(Controller);
		PC->AddYawInput(Value);
	}
}

void AShooterCharacter::LookUp(float Value)
{
	if (bAiming) {
		Value = Value * AimingSensetivityMultiplier;
	}
	if (Value != 0.f && Controller && Controller->IsLocalPlayerController())
	{
		APlayerController* const PC = CastChecked<APlayerController>(Controller);
		PC->AddPitchInput(Value);
	}
}

void AShooterCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr) 
	{
		return;
	}

	if (CombatState != ECombatState::ECS_Unoccupied) 
	{
		return;
	}
	if (WeaponHasAmmo()) 
	{
		PlayFireSound();
		SendBullet();
		PlayGunFireMontage();
		EquippedWeapon->DecrementAmmo();
		StartCrosshairBulletFire();
		StartFireTimer();
	}
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
	// Check for crosshair trace hit
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult,OutBeamLocation);

	if (bCrosshairHit)
	{
		//Tentative beam location - still need to trace from gun
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else// no crosshair trace hit
	{
		// OutBeamLocation is the End location for the line trace
	}

	
	//Preform second trace this time from the gun barrel
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation -MuzzleSocketLocation};
	const FVector WeaponTraceEnd{ MuzzleSocketLocation +StartToEnd*1.25f};
	GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);

	//Object Between barrel and BeamEndPoint?
	if (WeaponTraceHit.bBlockingHit)
	{
		OutBeamLocation = WeaponTraceHit.Location;
		return true;
	}
	return false;
}

void AShooterCharacter::AimingButtonPressed()
{
	bAiming = true;
	
	
}

void AShooterCharacter::AimingButtonReleased()
{
	bAiming = false;
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	//Calculating crosshair Velocity factor
	FVector2D WalkSpeedRange{ 0.f,600.f };
	FVector2D VelocityMultiplierRange{ 0.f,1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	//Calculate crosshair IsInAirFactor
	if (GetCharacterMovement()->IsFalling()) {//Is in air?
		//Spread the crosshairs slowly while in air
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else {//Character is on the ground
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f); //Shrink crosshair while in ground
	}
	//Calculate crosshair CrosshairAimFactor
	if (bAiming) 
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 5.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 2.25f, DeltaTime, 5.f);
	}

	// true 0.05 sec after firing
	if (bFiringBullet) {
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.15f, DeltaTime, 60.f);
	}
	else 
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}


	CrosshairSpreadMultiplier = 0.5f + 
		CrosshairVelocityFactor + 
		CrosshairInAirFactor+
		CrosshairAimFactor+
		CrosshairShootingFactor;
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;
	GetWorldTimerManager().SetTimer(CrosshairShootTimer,
		this, 
		&AShooterCharacter::FinishCrosshairBulletFire, 
		ShootTimeDuration);
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

void AShooterCharacter::FireButtonPressed()
{
	bFireBottonPressed = true;
	FireWeapon();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireBottonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	CombatState = ECombatState::ECS_FireTimerInProgress;
	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, AutomaticFireRate);
	
}

void AShooterCharacter::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;
	if (WeaponHasAmmo()) 
	{
		if (bFireBottonPressed) 
		{
			FireWeapon();
		}
	}
	else 
	{
		//Reload Weapon
		ReloadWeapon();
		
	}
	

}

bool AShooterCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	//Get Viewport size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	//Get Screen location of Crosshairs
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	//Get world position and direction of crosshairs into CrosshairWorldPosition and CrosshairWorldDirection, return true if succed
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld) 
	{
		//Trace from crosshair world location outworld
		const FVector Start{ CrosshairWorldPosition };
		//start vector+ direction vector will give Point where we looking at
		const FVector End{ Start + CrosshairWorldDirection * 50'000 };
		OutHitLocation = End;
		//Create the line and store output into OutHitResult
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);
		if (OutHitResult.bBlockingHit) 
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector OutHitLocation;
		TraceUnderCrosshairs(ItemTraceResult, OutHitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
				TraceHitItem->EnableCustomDepth();
			}
			//We hit an AItem last frame
			if (TraceHitItemLastFrame) 
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					//We are hiting a different item this frame from last frame or the new item is null-> we need to turn off widget visibility last frame item
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
					TraceHitItemLastFrame->DisableCustomDepth();
				}
			}
			//Store a refrence for hit item for next frame
			TraceHitItemLastFrame = TraceHitItem;

		}
	}
	else if (TraceHitItemLastFrame) 
	{
		//no longer overlapping any items
		//Item last frame should not show widget
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		TraceHitItemLastFrame->DisableCustomDepth();
		
	}
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	//Check the TSubclass variable (Default weapon class)
	if (DefaultWeaponClass) 
	{
		//Spawn weapon
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip) 
	{
		//Get hand socket
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket) 
		{
			//Attach weapon to hand socket
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}
		//set currently equipped weapon to new weapon
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);

		
	}
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon) 
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void AShooterCharacter::SelectButtonPressed()
{
	if (TraceHitItem)
	{
		
		TraceHitItem->StartItemCurve(this);
		
	}
}

void AShooterCharacter::SelectButtonReleased()
{
	
	
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

void AShooterCharacter::InitializeAmmoMap()
{
	//Ammo map Initialize
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
	AmmoMap.Add(EAmmoType::EAT_Shotgun, StartingShotgunAmmo);

	//maxmimum capacity map Initialize
	CapacityAmmoMap.Add(EAmmoType::EAT_9mm, 400);
	CapacityAmmoMap.Add(EAmmoType::EAT_AR, 300);
	CapacityAmmoMap.Add(EAmmoType::EAT_Shotgun, 200);

}

bool AShooterCharacter::WeaponHasAmmo()
{
	if (EquippedWeapon == nullptr)
	{
		return false;
	}
	return EquippedWeapon->GetAmmo() > 0;
}

void AShooterCharacter::PlayFireSound()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
}

void AShooterCharacter::SendBullet()
{
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		/**
		* Spawn Muzzle Flash animation from barrel of the gun
		*/
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation()
			, BeamEnd);
		if (bBeamEnd) {
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEnd);
			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}

	}
}

void AShooterCharacter::PlayGunFireMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage) {
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

void AShooterCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterCharacter::ReloadWeapon()
{
	
	if (CombatState != ECombatState::ECS_Unoccupied) 
	{
		return;
	}

	if (EquippedWeapon == nullptr)
	{
		return;
	}
	//Dont need to Reload.
	if (EquippedWeapon->GetAmmo() == EquippedWeapon->GetMagazineCapacity()) 
	{
		return;
	}
	
	//Do we have ammo of currect Type?
	if (CarryingAmmo() && !EquippedWeapon->ClipIsFull())
	{
		CombatState = ECombatState::ECS_Reloading;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ReloadMontage) 
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}
	}
}

bool AShooterCharacter::CarryingAmmo()
{
	if (EquippedWeapon == nullptr) 
	{
		return false;
	}
	auto AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType)) 
	{
		return AmmoMap[AmmoType] > 0;
	}
	return false;
}

void AShooterCharacter::GrabClip()
{
	if (EquippedWeapon == nullptr) 
	{
		return;
	}
	if (HandSceneComponent == nullptr) 
	{
		return;
	}
	//Index on the clip bone on the Equipped weapon
	int32 ClipBoneIndex{EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName())};
	//Store The Transforme of the clip
	ClipTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("Hand_L")));
	HandSceneComponent->SetWorldTransform(ClipTransform);

	EquippedWeapon->SetMovingClip(true);
}

void AShooterCharacter::ReleaseClip()
{
	EquippedWeapon->SetMovingClip(false);
}

void AShooterCharacter::SetWalkSpeed()
{
	if (bCrouching) 
	{
		if (bAiming)
		{
			//Crouch && Aim
			GetCharacterMovement()->MaxWalkSpeed = 100.f;
		}
		else
		{
			//Crouch && Hip
			GetCharacterMovement()->MaxWalkSpeed = 400.f;
		}
	}
	else
	{
		if (bAiming)
		{
			//Standing && Aim
			GetCharacterMovement()->MaxWalkSpeed = 150.f;
		}
		else
		{
			//Standing && Hip
			GetCharacterMovement()->MaxWalkSpeed = 600.f;
		}
	}
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling()) 
	{
		bCrouching = !bCrouching;
	}
	if (bCrouching) 
	{
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
	}
	else 
	{
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
	
	
}

void AShooterCharacter::Jump()
{
	if (bCrouching) 
	{
		bCrouching = false;
	}
	else 
	{
		ACharacter::Jump();
	}
}

void AShooterCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{};
	if (bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else 
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}
	const float InterpHalfHeight{ FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),TargetCapsuleHalfHeight,DeltaTime,20.f) };

	//Negative value if crouching
	//Positive value if standing
	const float DeltaCapsuleHalfHeight{ InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight()};
	const FVector MeshOffset{ 0.f,0.f,-DeltaCapsuleHalfHeight };
	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

void AShooterCharacter::PickUpAmmo(AAmmo* Ammo)
{
	//Check to see if AmmoMap contains Ammo's AmmoType
	if (AmmoMap.Find(Ammo->GetAmmoType())) 
	{
		//Get Amount of ammo in our AmmoMap for Ammo's type
		int32 AmmoCount{ AmmoMap[Ammo->GetAmmoType()] };
		AmmoCount += Ammo->GetItemCount();
		//Normelize AmmoCount to storage capacity
		if (AmmoCount > CapacityAmmoMap[Ammo->GetAmmoType()]) 
		{
			AmmoCount = CapacityAmmoMap[Ammo->GetAmmoType()];

		}
		//Set The amout of ammo in the Map for this ammo type
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}
	if (EquippedWeapon->GetAmmoType () == Ammo->GetAmmoType())
	{
		//Check to see if the gun is empty
		if (EquippedWeapon->GetAmmo() == 0) 
		{
			ReloadWeapon();
		}
	}
	Ammo->Destroy();
	TraceHitItem = nullptr;

	TraceHitItemLastFrame = nullptr;
}

void AShooterCharacter::InitializeInterpLocations()
{
	FInterpLocation WeaponLocation{ WeaponInterpComp,0};
	InterpLocations.Add(WeaponLocation);

	FInterpLocation IntepLoc1{ InterpComp1,0 };
	InterpLocations.Add(IntepLoc1);

	FInterpLocation IntepLoc2{ InterpComp2,0 };
	InterpLocations.Add(IntepLoc2);

	FInterpLocation IntepLoc3{ InterpComp3,0 };
	InterpLocations.Add(IntepLoc3);

	FInterpLocation IntepLoc4{ InterpComp4,0 };
	InterpLocations.Add(IntepLoc4);

	FInterpLocation IntepLoc5{ InterpComp5,0 };
	InterpLocations.Add(IntepLoc5);

	FInterpLocation IntepLoc6{ InterpComp6,0 };
	InterpLocations.Add(IntepLoc6);
}

int32 AShooterCharacter::GetInterptLocationIndex()
{
	int32 LowestIndex = 1;
	int32 LowestCount = INT_MAX;

	for (int32 i = 1; i < InterpLocations.Num(); i++) 
	{
		if (InterpLocations[i].ItemCount < LowestCount) 
		{
			LowestIndex = i;
			LowestCount = InterpLocations[i].ItemCount;
		}
	}

	return LowestIndex;
}

void AShooterCharacter::IncrementInterpLocItemCount(int32 Index, int32 Amount)
{
	if (Amount < -1 || Amount > 1) 
	{
		return;
	}
	if (InterpLocations.Num() >= Index) 
	{
		InterpLocations[Index].ItemCount += Amount;
	}
}

void AShooterCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(PickupSoundTimer, this, &AShooterCharacter::ResetPickupSoundTimer, PickupSoundResetTime);
}

void AShooterCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundTimer, this, &AShooterCharacter::ResetEquipSoundTimer, EquipSoundResetTime);

	
}




// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Set Current Camera FOV for the zoom in
	SetCurrentFOV(DeltaTime);
	//Calculate crosshair spread multiplier
	CalculateCrosshairSpread(DeltaTime);
	//Check overlap item count, then trace for items
	TraceForItems();
	//Set Walk Speed base on aiming or not
	SetWalkSpeed();
	//Interpelate capsule half height base on crouching/standing
	InterpCapsuleHalfHeight(DeltaTime);


}
void AShooterCharacter::SetCurrentFOV(float DeltaTime) {
	if (bAiming && !bCrouching) {
		//Interpelate to zoomed FOV
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
		//Change look sensativity base on aiming/not aiming
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else if(bAiming && bCrouching)
	{
		//Interpelate to zoomed FOV
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CrouchingCameraZommedFOV, DeltaTime, ZoomInterpSpeed);
		//Change look sensativity base on aiming/not aiming
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else
	{
		////Interpelate to Default FOV
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
		//Change look sensativity base on aiming/not aiming
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);

	
	

	
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward",this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("Aiming", IE_Released, this, &AShooterCharacter::AimingButtonReleased);
	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this, &AShooterCharacter::SelectButtonReleased);
	PlayerInputComponent->BindAction("ReloadButton", IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed);

	
	

}

void AShooterCharacter::FinishReloading()
{
	//Update Combat state
	CombatState = ECombatState::ECS_Unoccupied;

	if (EquippedWeapon == nullptr) 
	{
		return;
	}
	const auto AmmoType{ EquippedWeapon->GetAmmoType() };
	//Update ammo map
	if (AmmoMap.Contains(AmmoType))
	{
		//Amount of ammo the character is carrying of the equipped weapon type
		int32 CarriedAmmo = AmmoMap[AmmoType];
		//Space left in the magazine in the equipped weapon
		const int32 MagEmptySpace = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();
		if (MagEmptySpace > CarriedAmmo) {
			//Reload the Magazine with all the ammo we are carrying
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else 
		{
			//Fill Magazine
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
}

void AShooterCharacter::ResetPickupSoundTimer()
{
	bShouldPlayPickupSound = true;
}

void AShooterCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}

float AShooterCharacter::getCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterCharacter::IncreamentOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else 
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}
/***
* 
No longer needed, AItem has GetInterpLocaiton
*
FVector AShooterCharacter::GetCameraInterpLocation()
{
	const FVector CameraWorldLocation{ FollowCamera->GetComponentLocation() };
	const FVector CameraForward{FollowCamera->GetForwardVector()};
	//Desired location = CameraWorldLocation + ForwardVector * A + Up * B
	return CameraWorldLocation + CameraForward * CameraInterpDistance + FVector (0.f , 0.f, CameraInterpElevation);
}
*/
void AShooterCharacter::GetPickUpItem(AItem* Item)
{
	if (Item == nullptr) 
	{
		return;
	}
	Item->PlayEquipSound();

	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon) 
	{
		SwapWeapon(Weapon);
	}
	auto Ammo = Cast<AAmmo>(Item);
	if (Ammo) 
	{
		PickUpAmmo(Ammo);
	}
}

FInterpLocation AShooterCharacter::GetInterptLocation(int32 Index)
{
	if (Index <= InterpLocations.Num()) 
	{
		return InterpLocations[Index];
	}

	return FInterpLocation();
}

