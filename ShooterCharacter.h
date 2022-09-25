// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"




UENUM(BlueprintType)
enum class ECombatState : uint8 {
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),

	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};

USTRUCT(BlueprintType)
struct FInterpLocation 
{
	GENERATED_BODY()

	//Scene Component for use to its Location for Interping
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USceneComponent* SceneComponent;

	//Number of Items interpeng to/at to this scene comp location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;

};

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//Called for forward/backward input
	void MoveForward(float Value);
	//Called for side to side input
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate
	* @param Rate This is a normalized rate, i.e 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to look up/down at a given rate.
	* @param Rate This is a normalized rate, i.e 1.0 means 100% of desired rate
	*/
	void LookUpAtRate(float Rate);

	/**
	* Rotate controller based on mouse X movement
	* @param Value The input Value from mouse movement
	*/
	void Turn(float Value);

	/**
	* Rotate controller based on mouse Y movement
	* @param Value The input Value from mouse movement
	*/
	void LookUp(float Value);
	/**
	* Called when fire button is pressed
	*/
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	/**Set bAiming to true or false*/
	void AimingButtonPressed();
	void AimingButtonReleased();

	/**Function to set current FOV smoothly, called from tick*/
	void SetCurrentFOV(float DeltaTime);

	/**
	* Calculate the current crosshair spread
	* @param DeltaTime
	*/
	void CalculateCrosshairSpread(float DeltaTime);

	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

	void FireButtonPressed();
	void FireButtonReleased();
	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	/**Line trace for items under crosshairs*/
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	/**Trace for items if OverlappedItemCount > 0*/
	void TraceForItems();

	/**Spawns Default Weapon in begin play*/
	class AWeapon* SpawnDefaultWeapon();

	/**Takes a weapon and attach it to the mesh*/
	void EquipWeapon(AWeapon* WeaponToEquip);

	/**Detach Weapon and let if fall to the ground*/
	void DropWeapon();

	void SelectButtonPressed();
	void SelectButtonReleased();

	/**Drops Currently equipped weapon and equips TraceHitItem */
	void SwapWeapon(AWeapon* WeaponToSwap);

	/**Initialize ammo map with ammo values*/
	void InitializeAmmoMap();

	

	/**Check to make sure our weapon has ammo*/
	bool WeaponHasAmmo();

	/**Fire Weapon Functions*/
	void PlayFireSound();
	void SendBullet();
	void PlayGunFireMontage();

	/**Bound to R Key and Gamepad face button left*/
	void ReloadButtonPressed();

	/**Handle Reloading of Weapon*/
	void ReloadWeapon();

	/**Check to see if we have ammo for Equipped Weapon Ammo type*/
	bool CarryingAmmo();

	/**Called from animation blueprint with Grab clip notify*/
	UFUNCTION(BlueprintCallable)
	void GrabClip();
	/**Called from animation blueprint with Release clip notify*/
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	void SetWalkSpeed();

	void CrouchButtonPressed();

	virtual void Jump() override;

	/**Interps Capsule half height while crouching or standing*/
	void InterpCapsuleHalfHeight(float DeltaTime);

	void PickUpAmmo(class AAmmo* Ammo);

	void InitializeInterpLocations();

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/**Camera That follows the character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/**Base turn rate, in deg/sec. Other scaling may affect final turn rate*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/**Base look up/down rate, in deg/sec. Other scaling may affect final turn rate*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	/**Turn rate while not aiming*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;

	/**Look up rate while not aiming*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;

	/**Turn rate while aiming*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;

	/**Look up rate while aiming*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingLookUpRate;

	/**The difference in sensetivity while aiming instead of hip sensetivity in mouse controlls*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingSensetivityMultiplier;


	/**The Sound affect for the gun*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta= (AllowPrivateAccess= "true"))
	class USoundCue* FireSound;

	/**flash spawned at barrel socket*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	/**Montage for firing weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	/**Impact Partical Effect spawned upon bullet impact*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	/**Smoke trail for bullets*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	/**True if aiming else false*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= Combat, meta = (AllowPrivateAccess= "true"))
	bool bAiming;

	float ShootTimeDuration;

	bool bFiringBullet;

	FTimerHandle CrosshairShootTimer;


	/**Default camera fov value*/
	float CameraDefaultFOV;

	/**FOV value when zoomed in*/
	float CameraZoomedFOV;

	float CrouchingCameraZommedFOV;

	/**Current FOV this frame*/
	float CameraCurrentFOV;

	/**Interp speed when zoom in when aiming*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	/**Determine the spread of the crosshair*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess= "true"))
	float CrosshairSpreadMultiplier;

	/**Velocity component for crosshair spread*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	/**In Air component for crosshair spread*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	/**Aim component for crosshair spread*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	/**Shooting component for crosshair spread*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	/**Fire buttons Pressed*/
	bool bFireBottonPressed;

	/**True when we can fire, false when w8 for timer*/
	bool bShouldFire;

	/**Rate of Autofire*/
	float AutomaticFireRate;

	/**Sets a timer between gunshots*/
	FTimerHandle AutoFireTimer;

	/**True if we should trace every frame for items*/
	bool bShouldTraceForItems;

	/**Number of overlapped AItems*/
	int8 OverlappedItemCount;

	/**The AItem we hit last frame*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category=Items, meta = (AllowPrivateAccess = "true"))
	class AItem* TraceHitItemLastFrame;

	/**Currently equipped weapon*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	/**Set this in blueprint for the deafualt weapon class*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/**The Item currently hit by our trace in TraceForItem (Could be null)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	/**Distance outward from camera from the interpt destination*/
	UPROPERTY(EditAnywhere , BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpDistance;

	/**Distance upward from camera from the interpt destination*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;

	/** Map to keep track of ammo of different ammo types*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	/**Map to maximum Capacity for each ammo type*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> CapacityAmmoMap;

	/**Starting amount of 9mm ammo*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;

	/**Starting amount of AR ammo*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;

	/**Starting amount of Shotgun ammo*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 StartingShotgunAmmo;

	/**Combat State, can only fire or reload when unoccupied*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	/**Montage for Reloading weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	/**Transform of the clip when we first grabing the clip when reloadinh*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransform;

	/**Scene component to attach to character hand during reloading to attach to it the clip*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	/**True when Crouching*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	/**Current half height for capsule*/
	float CurrentCapsuleHalfHeight;

	/**Capsule half height while Not crouching*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight;

	/**Capsule half height while crouching*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchingCapsuleHalfHeight;

	/**Ground friction while not crouching*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float BaseGroundFriction;

	/**Ground friction while crouching*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchingGroundFriction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponInterpComp;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp6;

	/**Array of interp locations structs*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FInterpLocation> InterpLocations;

	FTimerHandle PickupSoundTimer;
	FTimerHandle EquipSoundTimer;

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;

	
	void ResetPickupSoundTimer();

	
	void ResetEquipSoundTimer();

	/**Time to wait before we can play another pickup sound*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float PickupSoundResetTime;

	/**Time to wait before we can play another Equip sound*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float EquipSoundResetTime;





public:
	/**Returns Camera Boom subobject*/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/**Returns FollowCamera subobject*/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool getAiming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
	float getCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	

	/**Addes/Substruct to/from OverlappedItemCount and updates bShouldTraceForItems*/
	void IncreamentOverlappedItemCount(int8 Amount);

	/**Get Camera InterpLocation for item pick up animaiton*/
	//No longer needed, AItem has GetInterpLocaiton
	//FVector GetCameraInterpLocation();

	void GetPickUpItem(AItem* Item);

	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }

	FORCEINLINE bool GetCrouching() const { return bCrouching; }

	FORCEINLINE TMap<EAmmoType, int32> GetAmmoMap() const { return AmmoMap; }
	FORCEINLINE TMap<EAmmoType, int32> GetCapacityAmmoMap() const { return CapacityAmmoMap; }

	FInterpLocation GetInterptLocation(int32 Index);

	/**Returns the index in the InterpLocation array with the lowest item count*/
	int32 GetInterptLocationIndex();

	void IncrementInterpLocItemCount(int32 Index, int32 Amount);

	FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; }
	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

	void StartPickupSoundTimer();
	void StartEquipSoundTimer();


};
