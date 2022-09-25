// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"




UShooterAnimInstance::UShooterAnimInstance() : 
	Speed(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	MovementOffsetYaw(0.f),
	LastMovementOffsetYaw(0.f),
	bAiming(false),
	CharacterYaw_TurnInPlace(0.f),
	CharacterYawLastFrame_TurnInPlace(0.f),
	RootYawOffset(0.f),
	Pitch(0.f),
	bReloading(false),
	OffsetState(EOffsetState::EOS_Hip),
	CharacterRotation(FRotator(0.f)),
	CharacterRotationLastFrame(FRotator(0.f)),
	YawDelta(0.f),
	bCrouching(false),
	RecoilWeight(1.f),
	bTurningInPlace(false)
{
}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr) {
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter) {

		bCrouching = ShooterCharacter->GetCrouching();

		bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;

		//Get the lateral speed of the charracter from velocity
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		//Is the charachter is in the air?
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		//Is the character accelerating?
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f) {
			bIsAccelerating = true;
		}
		else 
		{
			bIsAccelerating = false;
		}


		//Return the rotation we are looking at
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();

		//Return the movement Rotation
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

		//Movement offset yaw is movement rotation - aim rotation = the direction of where we walk
		//We Use this when we want to know which animation to play when we walk.
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation , AimRotation).Yaw;

		//Saving last movement yaw for knowing which stop jog animation to play
		if (ShooterCharacter->GetVelocity().Size() > 0.f) {
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		bAiming = ShooterCharacter->getAiming();

		if (bReloading) 
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if (bIsInAir) 
		{
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if (ShooterCharacter->getAiming()) 
		{
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else 
		{
			OffsetState = EOffsetState::EOS_Hip;
		}
	}
	TurnInPlace();
	Lean(DeltaTime);
}
void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) 
	{
		return;
	}
	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;
	if (Speed > 0 || bIsInAir) 
	{
		//Character is moving, we dont want to turn in place
		RootYawOffset = 0.f;
		CharacterYaw_TurnInPlace = ShooterCharacter->GetActorRotation().Yaw;
		CharacterYawLastFrame_TurnInPlace = CharacterYaw_TurnInPlace;
		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;

	}
	else 
	{
		CharacterYawLastFrame_TurnInPlace = CharacterYaw_TurnInPlace;
		CharacterYaw_TurnInPlace = ShooterCharacter->GetActorRotation().Yaw;
		const float YawDelta_TIP{ CharacterYaw_TurnInPlace - CharacterYawLastFrame_TurnInPlace };

		//Root Yaw offset updated and clampt to [-180,180]
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - YawDelta_TIP);

		const float Turning{ GetCurveValue(TEXT("Turning")) };
		// 1.0 if turning, 0.0 if not
		if (Turning > 0) 
		{
			bTurningInPlace = true;
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			//The difference between Rotation curve in this frame and the previus frame
			const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

			//if RootYawOffset > 0 -> we are turning left
			//if RootYawOffset < 0 -> we are turning right
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation: RootYawOffset += DeltaRotation;

			const float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };
			if (ABSRootYawOffset > 90) 
			{
				const float YawExcess{ ABSRootYawOffset - 90.f };
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
		else 
		{
			bTurningInPlace = false;
		}
		
	}
	RecoilCalculation();
}

void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (ShooterCharacter == nullptr) 
	{
		return;
	}
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();

	const FRotator Delta{ UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation,CharacterRotationLastFrame) };

	const float Target{ (float)Delta.Yaw / DeltaTime};

	const float Interp{ FMath::FInterpTo(YawDelta,Target,DeltaTime,6.f) };

	YawDelta = FMath::Clamp(Interp, -90.f, 90.f);
}

void UShooterAnimInstance::RecoilCalculation()
{
	if (bTurningInPlace)
	{
		if (bReloading)
		{
			RecoilWeight = 1.f;
		}
		RecoilWeight = 0.f;
	}
	else //not turning
	{
		if (bCrouching)
		{
			if (bReloading)
			{
				RecoilWeight = 1.f;
			}
			else
			{
				RecoilWeight = 0.1f;
			}
		}
		else
		{
			if (bAiming || bReloading)
			{
				RecoilWeight = 1.f;
			}
			else
			{
				RecoilWeight = 0.7f;
			}
		}
	}
}
