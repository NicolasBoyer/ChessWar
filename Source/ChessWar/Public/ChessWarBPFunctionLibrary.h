// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "ChessWarBPFunctionLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(GChessWarBPLibrary, Log, All);

/**
 * 
 */
UCLASS()
class CHESSWAR_API UChessWarBPFunctionLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Set Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintCallable, Category = "ChessWar | ChessWarBPLibrary")
	static void SetCustomConfigVar_String(FString SectionName, FString VariableName, FString Value);

	/** Get Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintPure, Category = "ChessWar | ChessWarBPLibrary")
	static FString GetCustomConfigVar_String(FString SectionName, FString VariableName);

	/** Set Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintCallable, Category = "ChessWar | ChessWarBPLibrary")
	static void SetCustomConfigVar_Int(FString SectionName, FString VariableName, int32 Value);

	/** Get Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintPure, Category = "ChessWar | ChessWarBPLibrary")
	static int32 GetCustomConfigVar_Int(FString SectionName, FString VariableName);

	/** Returns three arrays containing all of the resolutions and refresh rates for the current computer's current display adapter. You can loop over just 1 of the arrays and use the current index for the other two arrays, as all 3 arrays will always have the same length. Returns false if operation could not occur. */
	UFUNCTION(BlueprintPure, Category = "ChessWar | ChessWarBPLibrary")
	static bool GetDisplayAdapterScreenResolutions(TArray<int32>& Widths, TArray<int32>& Heights, TArray<int32>& RefreshRates, bool IncludeRefreshRates = false);

	/** Get Max possible Resolution os the screen player */
	UFUNCTION(BlueprintCallable, Category = "ChessWar | ChessWarBPLibrary")
	static void GetMaxScreenResolution(int32& Width, int32& Height);

	UFUNCTION(BlueprintPure, Category = "ChessWar | ChessWarBPLibrary")
	static FVector GetStaticMeshSize(class UStaticMesh* Mesh);

	/**
	* Rotate an actor (a pawn for example) in Yaw or Pitch direction with the SpeedRotate speed. The controller can be a Player or an AI Controller.
	*
	* @param		Controller		The controller of the rotate action
	* @param		Actor			The actor to rotate
	* @param		Yaw				Yaw direction (left to right)
	* @param		Pitch			Pitch direction (top to bottom)
	* @param		SpeedRotate		The speed of the rotation
	*/
	UFUNCTION(BlueprintCallable, Category = "ChessWar | ChessWarBPLibrary")
	static void RotateActor(AController* Controller, AActor* Actor, const float Yaw, const float Pitch, const float SpeedRotate);

	/**
	* Move an actor (a pawn for example) in Forward or Right direction with the SpeedMove speed. InverseDirection can inverse the direction of the move action.
	*
	* @param		Actor				The actor to move
	* @param		Right				Right direction
	* @param		Forward				Forward direction
	* @param		Up					Up direction
	* @param		SpeedMove			The speed of the move
	* @param		InverseDirection	Inverse the direction of the movement (Back for Forward, Left for Right, Bottom for Up). False by default
	*/
	UFUNCTION(BlueprintCallable, Category = "ChessWar | ChessWarBPLibrary")
	static void MoveActor(AActor* Actor, const float Right, const float Forward, const float Up, const float SpeedMove, const bool InverseDirection = false);

	/**
	* Zoom on a camera of an actor (a pawn for example) with the Player Controller.
	*
	* @param		PlayerController		The controller of the zoom
	* @param		Actor					The actor who own the camera
	* @param		SpringArm				The Spring Arm of the Camera to move when we zoom
	* @param		ZoomPosition			The position of the zoom
	* @param		SpeedZoom				The speed of the zoom
	* @param		ZoomMin					Minimum zoom of the camera (Near)
	* @param		ZoomMax					Maximum zoom of the camera (Far)
	* @param		MinCameraAngle			Min angle of the camera when pitch on zoom is active
	* @param		MaxCameraAngle			Max angle of the camera when pitch on zoom is active
	* @param		PitchOnZoom				Is the zoom pitch to the top when zoom. True by default
	* @param		ZoomOnMousePosition		Is the zoom center on the mouse cursor when zoom. True by default
	* @return		ZoomPosition			Return the new position of the camera. Use for a smooth camera on a tick event
	*/
	UFUNCTION(BlueprintCallable, Category = "ChessWar | ChessWarBPLibrary")
	static void ZoomOnCamera(APlayerController* PlayerController, AActor* Actor, USpringArmComponent* SpringArm, float& ZoomPosition, const float SpeedZoom, const float ZoomMin, const float ZoomMax, const float MinCameraAngle,
	                         const float MaxCameraAngle, const bool PitchOnZoom = true, const bool ZoomOnMousePosition = true);

	UFUNCTION(BlueprintCallable, Category = "ChessWar | ChessWarBPLibrary")
	static void CreateGrid(AActor* Actor, UStaticMesh* Mesh, FVector& ScaledMeshSize, TArray<FVector2D>& LocalPositions, const int32 TileX, const int32 TileY, UMaterialInterface* Material, const FName ColorParameter,
	                       const FVector Scale = FVector(1.f, 1.f, 1.f), const FLinearColor Color1 = FLinearColor(0.f, 0.f, 0.f), const FLinearColor Color2 = FLinearColor(0.f, 0.f, 0.f));

private:
	/** Reset the global counter for the ZoomOnCamera function */
	UFUNCTION()
	static void ResetZoomCnt();
};
