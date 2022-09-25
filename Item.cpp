// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Ammo.h"

// Sets default values
AItem::AItem():
	ItemName(FString("Default")),
	ItemCount(1),
	ItemRareity(3),
	ItemState(EItemState::EIS_Pickup),
	//Item Interp Variables
	ZCurveTime(0.7f),
	ItemInterpStartLocation(FVector(0.f)),
	CameraTargetLocation(FVector(0.f)),
	bInterping(false),
	ItemInterpX(0.f),
	ItemInterpY(0.f),
	InterpInitialYawOffset(0.f),
	ItemType(EItemType::EIT_MAX),
	InterpLocIndex(0),
	MaterialIndex(0),
	bCanChangeCustomDepth(true)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	AreaSphere->SetupAttachment(GetRootComponent());

	//Set Custom depth to disabled
	InitializeCustomDepth();
	

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	//hide widget at default, unhide when looking at
	PickupWidget->SetVisibility(false);

	//Setup overlap for area sphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
	//Set Item properties based on item state
	SetItemProperties(ItemState);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) 
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter) 
		{
			ShooterCharacter->IncreamentOverlappedItemCount(1);
			
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncreamentOverlappedItemCount(-1);
			
		}
	}
}

void AItem::SetItemProperties(EItemState State)
{
	switch (State) 
	{
	case EItemState::EIS_Pickup:
		//Set Mesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Set Area sphere properties
		//Turning On Area sphere
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		//Set Collision Box properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	case EItemState::EIS_Equipped:
		PickupWidget->SetVisibility(false);
		//Set Mesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Set Area sphere properties
		//Turning off area sphere
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Set Collision Box properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Falling:
		//Set Mesh Properties
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		//Set Area sphere properties
		//Turning off area sphere
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Set Collision Box properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_EquipInterping:
		PickupWidget->SetVisibility(false);
		//Set Mesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Set Area sphere properties
		//Turning off area sphere
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Set Collision Box properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	}
}

void AItem::FinishInterping()
{
	bInterping = false;
	if (Character) 
	{
		Character->GetPickUpItem(this);
		//Substruct 1 from the Item Count of the inrerp location struct
		Character->IncrementInterpLocItemCount(InterpLocIndex, -1);
	}
	//Set Scale Back to normal
	SetActorScale3D(FVector(1.f));

	

	DisableGlowMaterial();
	bCanChangeCustomDepth = true;
	DisableCustomDepth();

	
}

void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping)
	{
		return;
	}

	if (Character && ItemZCurve) 
	{
		//Time that have passed since we started our timer
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		//Get Curve value corresponding to ElapsedTime
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
		//Get the Item's initial location when the curve starts
		FVector ItemLocation = ItemInterpStartLocation;
		//Get Location in fromt of the camera
		const FVector CameraInterpLocation{ GetInterpLocation()};
		//Vector from item to camera interp location, X and Y are zerod out
		const FVector ItemToCamera{ FVector(0.f,0.f,(CameraInterpLocation - ItemLocation).Z) };
		//Scale factor to multiply with curve value
		const float DeltaZ = ItemToCamera.Size();

		//Interpolated X Value
		const FVector CurrentLocation{ GetActorLocation() };
		const float InterpXValue = FMath::FInterpTo(
			CurrentLocation.X,
			CameraInterpLocation.X, 
			DeltaTime, 
			30.0f);
		//Interpolated Y Value
		const float InterpYValue = FMath::FInterpTo(CurrentLocation.Y,
			CameraInterpLocation.Y,
			DeltaTime,
			30.0f);

		//Set X and Y of ItemLocation to interped Values
		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;

		//Adding curve value to the z component of the initial Location (Scaled by DeltaZ)
		ItemLocation.Z += CurveValue * DeltaZ;

		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);
		//Camera Rotataion this frame
		const FRotator CameraRotation{ Character->GetFollowCamera()->GetComponentRotation() };
		//Camera Rotataon + initial Yaw offset will make the Item allined with camera
		FRotator ItemRotation{ 0.f,CameraRotation.Yaw + InterpInitialYawOffset, 0.f };
		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);


		if (ItemScaleCurve) 
		{
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}


	}
}

FVector AItem::GetInterpLocation()
{
	if (Character == nullptr) 
	{
		return FVector{ 0.f };
	}
	switch (ItemType) 
	{
	case EItemType::EIT_Ammo:
		return Character->GetInterptLocation(InterpLocIndex).SceneComponent->GetComponentLocation();
		break;
	case EItemType::EIT_Weapon:
		return Character->GetInterptLocation(0).SceneComponent->GetComponentLocation();
		break;
	}
	return FVector();
}

void AItem::PlayPickupSound()
{
	if (Character) 
	{
		if (Character->ShouldPlayPickupSound()) 
		{
			Character->StartPickupSoundTimer();
			if (PickupSound) 
			{
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
	}
}

void AItem::EnableCustomDepth()
{
	if (bCanChangeCustomDepth) 
	{
		ItemMesh->SetRenderCustomDepth(true);
	}
}

void AItem::DisableCustomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemMesh->SetRenderCustomDepth(false);
	}
}

void AItem::InitializeCustomDepth()
{
	DisableCustomDepth();
}

void AItem::OnConstruction(const FTransform& Transform)
{
	if (MaterialInstance) 
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
		ItemMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);
		EnableGlowMaterial();
	}
}

void AItem::EnableGlowMaterial()
{
	if (DynamicMaterialInstance) 
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 0);
	}
}

void AItem::DisableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 1);
	}
}

void AItem::PlayEquipSound()
{
	if (Character)
	{
		if (Character->ShouldPlayEquipSound())
		{
			Character->StartEquipSoundTimer();
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Handle Item Interping when in the EquipInterping State
	ItemInterp(DeltaTime);


}
void AItem::SetItemState(EItemState State) 
{
	ItemState = State;
	SetItemProperties(State);
}

void AItem::StartItemCurve(AShooterCharacter* Char)
{
	//Store a handle to character
	Character = Char;

	//Get array index in InterpLocations with the lowest Item count
	InterpLocIndex = Character->GetInterptLocationIndex();
	//Add 1 to the Item Count for this interp location struct
	Character->IncrementInterpLocItemCount(InterpLocIndex, 1);
	
	//Check if the traced item is ammo
	AAmmo* AmmoItem = Cast<AAmmo>(this);
	if (AmmoItem)
	{
		if (Char->GetAmmoMap().Find(AmmoItem->GetAmmoType()))
		{
			//if it is ammo check if we have storage capacity
			if (Char->GetAmmoMap()[AmmoItem->GetAmmoType()] >= Char->GetCapacityAmmoMap()[AmmoItem->GetAmmoType()])
			{
				return;
			}
		}

	}

	//Handle pickup sound logic
	PlayPickupSound();

	//Starting location of item 
	ItemInterpStartLocation = GetActorLocation();
	bInterping = true;
	SetItemState(EItemState::EIS_EquipInterping);
	//Start timer for interping, when finished call FinishInterping function
	GetWorldTimerManager().SetTimer(
		ItemInterpTimer,
		this,
		&AItem::FinishInterping,
		ZCurveTime);

	//Get Iinitial Yaw of camera
	 const float CameraRotationYaw = Character->GetFollowCamera()->GetComponentRotation().Yaw ;
	//Get Iinitial Yaw of Item
	 const float ItemRotationYaw= GetActorRotation().Yaw ;
	//Initial Yaw offset between camera and item
	InterpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;

	bCanChangeCustomDepth = false;
}

