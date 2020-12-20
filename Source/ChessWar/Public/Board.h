// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChessWarCharacter.h"
#include "Board.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(GBoard, Log, All);

UCLASS()
class CHESSWAR_API ABoard final : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	// ABoard();
	explicit ABoard(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChessWar | ChessWarBoard")
	FVector MeshSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChessWar | ChessWarBoard")
	TArray<FVector2D> TMeshPositions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChessWar | ChessWarBoard")
	TArray<AChessWarCharacter*> TPieces;

	UFUNCTION(BlueprintPure, Category = "ChessWar | ChessWarBoard")
	bool IsPossibleMovementFromTarget(TArray<FVector2D> PossiblePositions, float TargetX, float TargetY, FVector2D& NewRookPositions, AChessWarCharacter*& RookForCastlingMvt, bool& IsPawnPromotion);

	UFUNCTION(BlueprintPure, Category = "ChessWar | ChessWarBoard")
	bool IsEmptyPosition(const FVector2D Position) const;

	UFUNCTION(BlueprintPure, Category = "ChessWar | ChessWarBoard")
	AChessWarCharacter* GetPieceToCaptureOnTarget() const;

	UFUNCTION(BlueprintCallable, Category = "ChessWar | ChessWarBoard")
	void GetCheckState(AChessWarCharacter* CurrentKing, bool& bIsCheckMate, bool& bIsStaleMate, bool& bIsCheck);

	UFUNCTION()
	AChessWarCharacter* GetPieceFromPosition(const FVector2D Position) const;

	UFUNCTION()
	bool IsCheck(AChessWarCharacter* SelectedPiece, FVector2D PossiblePosition);

	UFUNCTION()
	bool IsOutBoardOnYPosition(const float PosY) const;

	/**
	* Test si un vecteur est situé entre deux autres vecteurs, verticalement, horizontalement et en diagonale.
	*
	* @param		Position			Le vecteur à tester
	* @param		PosA				Premier vecteur entre lequel le test est fait
	* @param		PosB				Deuxième vecteur entre lequel le test est fait
	* @param		IncludeA			Si true, inclus PosA
	* @param		IncludeB			Si true, inclus PosB
	*/
	UFUNCTION()
	static bool IsPositionBetweenPosAAndPosB(const FVector2D Position, const FVector2D PosA, const FVector2D PosB, const bool IncludeA = false, const bool IncludeB = false);

	UPROPERTY()
	AChessWarCharacter* PieceToCapture;

	UPROPERTY()
	AChessWarCharacter* RookToMoveInCastlingMovement;

	UPROPERTY()
	AChessWarCharacter* PawnToCaptureInEnPassantMovement;

	UPROPERTY()
	AChessWarCharacter* PawnToPromote;

	UPROPERTY()
	FVector2D RookPositionsAfterCastlingMovement;

	UPROPERTY()
	FVector2D KingPositionForCastlingMovement;

	UPROPERTY()
	FVector2D PawnPositionForEnPassantMovement;

	UPROPERTY()
	bool bCheck;

protected:
	// Called when the game starts or when spawned
	// virtual void BeginPlay() override;

public:
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	AChessWarCharacter* ACurrentKing;
};
