// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessWarBPFunctionLibrary.h"
#include "RHI.h"
#include "Runtime/Core/Public/Core.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMesh.h"
#include "Engine/Public/TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Classes/Materials/MaterialInstanceDynamic.h"

DEFINE_LOG_CATEGORY(GChessWarBPLibrary);

/** Set the global counter for the ZoomOnCamera function */
int32 GZoomCnt = 0;

void UChessWarBPFunctionLibrary::SetCustomConfigVar_String(FString SectionName, FString VariableName, FString Value)
{
	if (!GConfig)
	{
		return;
	}

	GConfig->SetString(*SectionName, *VariableName, *Value, GGameIni);
}

FString UChessWarBPFunctionLibrary::GetCustomConfigVar_String(FString SectionName, FString VariableName)
{
	if (!GConfig)
	{
		return "";
	}

	FString Value;
	GConfig->GetString(*SectionName, *VariableName, Value, GGameIni);

	return Value;
}

void UChessWarBPFunctionLibrary::SetCustomConfigVar_Int(FString SectionName, FString VariableName, int32 Value)
{
	if (!GConfig)
	{
		return;
	}

	GConfig->SetInt(*SectionName, *VariableName, Value, GGameIni);
}

int32 UChessWarBPFunctionLibrary::GetCustomConfigVar_Int(FString SectionName, FString VariableName)
{
	if (!GConfig)
	{
		return 0;
	}

	int32 Value;
	GConfig->GetInt(*SectionName, *VariableName, Value, GGameIni);

	return Value;
}

bool UChessWarBPFunctionLibrary::GetDisplayAdapterScreenResolutions(TArray<int32>& Widths, TArray<int32>& Heights, TArray<int32>& RefreshRates, bool IncludeRefreshRates /*= false*/)
{
	//Clear any Previous
	Widths.Empty();
	Heights.Empty();
	RefreshRates.Empty();

	TArray<FString> Unique;

	FScreenResolutionArray Resolutions;
	if (RHIGetAvailableResolutions(Resolutions, false))
	{
		for (const FScreenResolutionRHI& EachResolution : Resolutions)
		{
			FString Str = "";
			Str += FString::FromInt(EachResolution.Width);
			Str += FString::FromInt(EachResolution.Height);

			//Include Refresh Rates?
			if (IncludeRefreshRates)
			{
				Str += FString::FromInt(EachResolution.RefreshRate);
			}

			if (Unique.Contains(Str))
			{
				//Skip! This is duplicate!
				continue;
			}
			else
			{
				//Add to Unique List!
				Unique.AddUnique(Str);
			}

			//Add to Actual Data Output!
			Widths.Add(EachResolution.Width);
			Heights.Add(EachResolution.Height);
			RefreshRates.Add(EachResolution.RefreshRate);
		}

		return true;
	}
	return false;
}

void UChessWarBPFunctionLibrary::GetMaxScreenResolution(int32& Width, int32& Height)
{
	TArray<int32> Widths;
	TArray<int32> Heights;
	TArray<int32> RefreshRates;

	if (GetDisplayAdapterScreenResolutions(Widths, Heights, RefreshRates))
	{
		for (const auto& WidthIt : Widths)
		{
			Width = WidthIt;
		}
		for (const auto& HeightIt : Heights)
		{
			Height = HeightIt;
		}
	}
}

FVector UChessWarBPFunctionLibrary::GetStaticMeshSize(class UStaticMesh* Mesh)
{
	if (!Mesh)
	{
		return FVector::ZeroVector;
	}

	return Mesh->GetBounds().GetBox().GetSize();
}

void UChessWarBPFunctionLibrary::RotateActor(AController* Controller, AActor* Actor, const float Yaw, const float Pitch, const float SpeedRotate)
{
	if (Yaw != 0.f || Pitch != 0.f)
	{
		const FRotator Rotation = {Pitch * SpeedRotate + Controller->GetControlRotation().Pitch, Yaw * SpeedRotate + Controller->GetControlRotation().Yaw, Controller->GetControlRotation().Roll};

		Actor->SetActorRotation(Rotation);
		Controller->SetControlRotation(Rotation);
	}
}

void UChessWarBPFunctionLibrary::MoveActor(AActor* Actor, const float Right, const float Forward, const float Up, const float SpeedMove, const bool InverseDirection /*= false*/)
{
	if (Forward != 0.f || Right != 0.f || Up != 0.f)
	{
		const auto RightMovement = SpeedMove * Right * (InverseDirection ? -1 : 1);
		const auto ForwardMovement = SpeedMove * Forward * (InverseDirection ? -1 : 1);
		const auto UpMovement = SpeedMove * Up * (InverseDirection ? -1 : 1);

		auto Location = Actor->GetRootComponent()->GetForwardVector() * ForwardMovement + Actor->GetRootComponent()->GetRightVector() * RightMovement + Actor->GetRootComponent()->GetUpVector() * UpMovement + Actor->
			GetActorLocation();
		Location = {Location.X, Location.Y, Up != 0.f ? Location.Z : Actor->GetActorLocation().Z};

		Actor->SetActorLocation(Location);
	}
}

void UChessWarBPFunctionLibrary::ZoomOnCamera(APlayerController* PlayerController, AActor* Actor, USpringArmComponent* SpringArm, float& ZoomPosition, const float SpeedZoom, const float ZoomMin, const float ZoomMax,
                                              const float MinCameraAngle, const float MaxCameraAngle, const bool PitchOnZoom /*= true*/, const bool ZoomOnMousePosition /*= true*/)
{
	ZoomPosition = FMath::Clamp(SpringArm->TargetArmLength + SpeedZoom, ZoomMin, ZoomMax);

	if (PitchOnZoom)
	{
		const auto CameraPitch = (FMath::Abs(MaxCameraAngle) - FMath::Abs(MinCameraAngle)) / ((ZoomMax - ZoomMin) / -SpeedZoom);
		const FRotator Rotation = {FMath::Clamp(SpringArm->GetRelativeRotation().Pitch + CameraPitch, MaxCameraAngle, MinCameraAngle), SpringArm->GetRelativeRotation().Yaw, SpringArm->GetRelativeRotation().Roll};
		SpringArm->SetRelativeRotation(Rotation);
	}

	if (ZoomOnMousePosition)
	{
		if (GZoomCnt == 0)
		{
			FHitResult RV_Hit(ForceInit);
			PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, RV_Hit);
			Actor->GetRootComponent()->SetWorldLocation(RV_Hit.Location);
		}
		GZoomCnt++;
		FTimerHandle TimerResetHandle;
		Actor->GetWorld()->GetTimerManager().SetTimer(TimerResetHandle, FTimerDelegate::CreateStatic(UChessWarBPFunctionLibrary::ResetZoomCnt), 0.5f, false);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::SanitizeFloat(WaitTime));
	}
}

void UChessWarBPFunctionLibrary::CreateGrid(AActor* Actor, UStaticMesh* Mesh, FVector& ScaledMeshSize, TArray<FVector2D>& LocalPositions, const int32 TileX, const int32 TileY, UMaterialInterface* Material,
                                            const FName ColorParameter, const FVector Scale, const FLinearColor Color1, const FLinearColor Color2)
{
	if (Mesh->IsValidLowLevel() && Actor->IsValidLowLevel())
	{
		const auto GridNumber = TileX * TileY;
		const auto MeshSize = GetStaticMeshSize(Mesh);
		ScaledMeshSize = MeshSize * Scale;
		// auto x = 0, y = 0;

		for (auto i = 0; i < GridNumber; i++)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::FromInt(i));
			//UE_LOG(ChessWarBPLibrary, Warning, TEXT("Mesh %d"),i);
			const auto y = i / TileX;
			const auto x = i - TileX * y;
			LocalPositions.Add(FVector2D(ScaledMeshSize.X * x, -ScaledMeshSize.Y * y));

			auto StaticMeshComponentTransform = FTransform(Actor->GetTransform().GetRotation(), FVector(ScaledMeshSize.X * x, -ScaledMeshSize.Y * y, 0.f), Scale);

			auto StaticMeshComponent = NewObject<UStaticMeshComponent>(Actor);
			StaticMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			StaticMeshComponent->RegisterComponent();
			StaticMeshComponent->SetStaticMesh(Mesh);
			StaticMeshComponent->AttachToComponent(Actor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			StaticMeshComponent->SetRelativeTransform(StaticMeshComponentTransform);

			if (Material)
			{
				StaticMeshComponent->SetMaterial(0, Material);
				auto InstanceMaterial = StaticMeshComponent->CreateAndSetMaterialInstanceDynamic(Material->GetLinkerIndex());
				InstanceMaterial->SetVectorParameterValue(ColorParameter, (FMath::Fmod(x, 2) == 0 && FMath::Fmod(y, 2) != 0) || (FMath::Fmod(x, 2) != 0 && FMath::Fmod(y, 2) == 0) ? Color1 : Color2);
			}
		}
	}
}

void UChessWarBPFunctionLibrary::ResetZoomCnt()
{
	GZoomCnt = 0;
}
