// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API AAmmo : public AItem
{
	GENERATED_BODY()
public:
	/**Constructor*/
	AAmmo();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/**Overrive of SetItemProperties so we can set AmmoMesh properties*/
	virtual void SetItemProperties(EItemState State) override;

	/**Called When overlapping area Ammo sphere*/
	UFUNCTION()
		void AmmoSphereOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	
	

private:
	/**Mesh for the Ammo pickup*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AmmoMesh;

	/**Ammo type for the ammo*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	/**Texure for the Ammo Icon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIconTexure;

	/**Overlap sphere for picking up ammo*/
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Ammo ,meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AmmoCollisionSphere;
	
public:
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	virtual void EnableCustomDepth() override;
	virtual void DisableCustomDepth() override;
	
};
