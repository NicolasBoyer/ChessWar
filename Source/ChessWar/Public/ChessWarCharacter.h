// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ChessWarCharacter.generated.h"

class ABoard;

UENUM(BlueprintType, Category = "ChessWar | ChessWarCharacter")
enum class EPiecesType : uint8
{
	E_Bishop UMETA(DisplayName = "Bishop"),
	E_King UMETA(DisplayName = "King"),
	E_Knight UMETA(DisplayName = "Knight"),
	E_Pawn UMETA(DisplayName = "Pawn"),
	E_Queen UMETA(DisplayName = "Queen"),
	E_Rook UMETA(DisplayName = "Rook")
};

UENUM()
enum class ECounterPosition : uint8
{
	E_InitOnBoard UMETA(DisplayName = "InitOnBoard"),
	E_OutBoard UMETA(DisplayName = "OutBoard"),
	E_OnBoard UMETA(DisplayName = "OnBoard")
};

UCLASS()
class CHESSWAR_API AChessWarCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AChessWarCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChessWar | ChessWarCharacter")
	bool bIsBlack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChessWar | ChessWarCharacter")
	bool bIsTwoSquareFirstMvt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChessWar | ChessWarCharacter")
	bool bIsMoved;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChessWar | ChessWarCharacter")
	bool bIsSelected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChessWar | ChessWarCharacter")
	EPiecesType EPieceType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChessWar | ChessWarCharacter")
	FVector2D FCurrentPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChessWar | ChessWarCharacter")
	TArray<FVector2D> TPossiblePositions;

	UPROPERTY()
	bool bIsProtectingKing;

	UPROPERTY()
	bool bHasChecked;

	UPROPERTY()
	bool bIsProtected;

	UPROPERTY()
	TArray<FVector2D> TDefaultMovements;

	UFUNCTION(BlueprintCallable, Category = "ChessWar | ChessWarCharacter")
	void SetDatas(const FVector2D Position, const bool IsBlack, EPiecesType PieceType, const bool IsMoved, ABoard* Board);

	UFUNCTION(BlueprintCallable, Category = "ChessWar | ChessWarBoard")
	void ChangePosition(const FVector2D Position, bool& IsEnPassantMvt);

	UFUNCTION(BlueprintCallable, Category = "ChessWar | ChessWarCharacter")
	void DeleteFromBoard(const bool DestroyPiece = true);

	UFUNCTION(BlueprintCallable, Category = "ChessWar | ChessWarCharacter")
	AChessWarCharacter* GetNextAvailablePieceOnBoard(float MoveX, float MoveY, FVector LocalMeshSize, float PosX = 0.f, float PosY = 0.f, int32 Counter = 0, ECounterPosition CounterPosition = ECounterPosition::E_InitOnBoard,
	                                                 int32 MaxCounter = 8) const;

	UFUNCTION()
	TArray<FVector2D> GetAllPossiblePositions();

	UFUNCTION()
	int32 GetPiecesNumberBetweenTwoPositionsInDefaultMvt(const FVector2D PosA, const FVector2D PosB) const;

private:
	UPROPERTY()
	ABoard* AChessWarBoard;

	UFUNCTION()
	void SetKnightPossiblePositions();

	UFUNCTION()
	void SetPawnPossiblePositions();

	UFUNCTION()
	void SetDiagonalPossiblePositions(const bool bIsOneSquareMovement = false);

	UFUNCTION()
	void SetHorizontalOrVerticalPositions(const bool bIsOneSquareMovement = false);

	UFUNCTION()
	void SetCastlingPositions();

	UFUNCTION()
	void SetPossiblePositions(const FVector2D PossiblePosition);

protected:
	// Called when the game starts or when spawned
	// virtual void BeginPlay() override;

public:
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	// virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
