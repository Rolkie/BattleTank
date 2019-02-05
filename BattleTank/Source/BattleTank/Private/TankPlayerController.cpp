// Fill out your copyright notice in the Description page of Project Settings.

#include "TankPlayerController.h"
#include "BattleTank.h"
#include "Runtime/Engine/Classes/Engine/World.h"


void ATankPlayerController::BeginPlay()
{
	
	Super::BeginPlay();

	auto ControlledTank = GetControlledTank();

	if (!ControlledTank)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController not possesing a tank!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController possesing: %s"), *ControlledTank->GetName());
	}
}

void ATankPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimTowardsCrosshair();
}

ATank* ATankPlayerController::GetControlledTank() const
{

	return Cast<ATank>(GetPawn());
}

void ATankPlayerController::AimTowardsCrosshair()
{
	if (!GetControlledTank())
	{
		return;
	}

	FVector HitLocation; //Out paramtere

	if (GetSightRayHitLocation(HitLocation)) // Raytraycing happens in this function.
	{
		GetControlledTank()->AimAt(HitLocation);
	}
}

//Get world location through crosshair, if hit the landscape return true
bool ATankPlayerController::GetSightRayHitLocation(FVector& HitLocation) const
{
	//Find crosshair position.
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	
	auto ScreenLocation = FVector2D(ViewportSizeX * CrossHairXLocation, ViewportSizeY * CrossHairYLocation);

	//De-project the screen position of the crosshair to world direction.
	FVector LookDirection;
	if (GetLookDirection(ScreenLocation, LookDirection))
	{
		//Line-trace along the LookDirection, and see what we hit (up to max range).
		GetLookVectorHitLocation(LookDirection, HitLocation);
	}

	return true;
}

bool ATankPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector& LookDirection) const
{
	FVector CameraWorldLocation; // To be discarded 

	return DeprojectScreenPositionToWorld(ScreenLocation.X, ScreenLocation.Y, CameraWorldLocation, LookDirection);
	
}

bool ATankPlayerController::GetLookVectorHitLocation(FVector LookDirection, FVector& HitLocation) const
{
	FHitResult HitResult;
	auto StartLocation = PlayerCameraManager->GetCameraLocation(); // Fetches the player camera location
	auto EndLocation = StartLocation + (LookDirection * LineTraceRange);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility))
	{
		HitLocation = HitResult.Location; // The location of the hit in world space
		return true;
	}
	HitLocation = FVector(0);
	return false;
}