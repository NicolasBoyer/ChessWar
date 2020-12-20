// Copyright 2019 Nicolas Boyer. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ChessWarCharacter.h"
#include "ChessWarAIController.generated.h"

UCLASS()
class CHESSWAR_API AChessWarAIController : public AAIController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "ChessWar | ChessWarBoard")
	static float GetEvaluationByPieceType(const EPiecesType EPieceType, const bool IsBlack, const int X, const int Y);
};
