// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessWarCharacter.h"
#include "Board.h"

// Sets default values
AChessWarCharacter::AChessWarCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	bIsTwoSquareFirstMvt = false;
	EPieceType = EPiecesType::E_Pawn;
	bIsBlack = false;
	bIsMoved = false;
	bIsSelected = false;
	FCurrentPosition = {0.f, 0.f};
	TPossiblePositions = {};
	TDefaultMovements = {};
	bIsProtectingKing = false;
	bHasChecked = false;
	bIsProtected = false;
}

void AChessWarCharacter::SetDatas(const FVector2D Position, const bool IsBlack, const EPiecesType PieceType, const bool IsMoved, ABoard* Board)
{
	bIsBlack = IsBlack;
	bIsMoved = IsMoved;
	FCurrentPosition = Position;
	EPieceType = PieceType;
	AChessWarBoard = Board;
	AChessWarBoard->TPieces.Add(this);
}

void AChessWarCharacter::ChangePosition(const FVector2D Position, bool& IsEnPassantMvt)
{
	if (IsValidLowLevel())
	{
		for (auto EachPiece : AChessWarBoard->TPieces)
		{
			EachPiece->bHasChecked = false;
			EachPiece->bIsProtected = false;
			// Reset bIsTwoSquareFirstMvt si égale à true
			if (EachPiece->bIsTwoSquareFirstMvt)
			{
				EachPiece->bIsTwoSquareFirstMvt = false;
			}
		}

		bIsMoved = true;

		// Retourne en ref si la nouvelle position est un mouvement en passant
		IsEnPassantMvt = AChessWarBoard->PawnPositionForEnPassantMovement == Position;

		// Si la pièce joué est un pion qui vient de bouger de deux case, on passe bIsTwoSquareFirstMvt à true pour permettre le coup "En passant"
		if (EPieceType == EPiecesType::E_Pawn && bIsMoved && (FCurrentPosition.Y - AChessWarBoard->MeshSize.Y * 2 == Position.Y || FCurrentPosition.Y + AChessWarBoard->MeshSize.Y * 2 == Position.Y))
		{
			bIsTwoSquareFirstMvt = true;
		}

		FCurrentPosition = Position;
	}
}

void AChessWarCharacter::DeleteFromBoard(const bool DestroyPiece)
{
	if (AChessWarBoard->TPieces.Contains(this))
	{
		AChessWarBoard->TPieces.Remove(this);
	}
	if (DestroyPiece) Destroy();
}

AChessWarCharacter* AChessWarCharacter::GetNextAvailablePieceOnBoard(float MoveX, float MoveY, FVector LocalMeshSize, float PosX /*= 0.f*/, float PosY /*= 0.f*/, int32 Counter /*= 0*/,
                                                                     ECounterPosition CounterPosition /*= ECounterPosition::E_InitOnBoard*/, int32 MaxCounter /*= 8*/) const
{
	if (!IsValidLowLevel())
	{
		return nullptr;
	}

	MoveX = !bIsBlack ? MoveX : -MoveX;
	MoveY = !bIsBlack ? MoveY : -MoveY;
	LocalMeshSize = LocalMeshSize.IsZero() ? AChessWarBoard->MeshSize : LocalMeshSize;

	PosX = PosX == 0.f ? FCurrentPosition.X + LocalMeshSize.X * MoveX : PosX;
	PosY = PosY == 0.f ? FCurrentPosition.Y + LocalMeshSize.Y * MoveY : PosY;
	auto Piece = AChessWarBoard->GetPieceFromPosition(FVector2D(PosX, PosY));

	if (MoveX != 0)
	{
		if (AChessWarBoard->IsEmptyPosition(FVector2D(PosX, PosY)) || Piece->IsValidLowLevel() && bIsBlack != Piece->bIsBlack || AChessWarBoard->IsOutBoardOnYPosition(PosY))
		{
			if (CounterPosition == ECounterPosition::E_InitOnBoard)
			{
				LocalMeshSize.Y = -LocalMeshSize.Y;
			}
			const auto MeshSizeY = !bIsBlack ? LocalMeshSize.Y : -LocalMeshSize.Y;
			Counter++;
			if (CounterPosition == ECounterPosition::E_InitOnBoard)
			{
				PosY = PosY + MeshSizeY * Counter;
			}
			else if (CounterPosition == ECounterPosition::E_OutBoard)
			{
				PosY = PosY - MeshSizeY * Counter;
				CounterPosition = ECounterPosition::E_OnBoard;
			}
			else
			{
				PosY = PosY - MeshSizeY;
			}
			if (AChessWarBoard->IsOutBoardOnYPosition(PosY))
			{
				CounterPosition = ECounterPosition::E_OutBoard;
			}
			if (Counter > 8)
			{
				Counter = 0;
				PosX = PosX + LocalMeshSize.X * MoveX;
				PosY = 0.f;
				CounterPosition = ECounterPosition::E_InitOnBoard;
			}
			Piece = GetNextAvailablePieceOnBoard(!bIsBlack ? MoveX : -MoveX, 0, LocalMeshSize, PosX, PosY, Counter, CounterPosition);
		}
	}

	if (MoveY != 0)
	{
		if (AChessWarBoard->IsEmptyPosition(FVector2D(PosX, PosY)) || Piece->IsValidLowLevel() && bIsBlack != Piece->bIsBlack)
		{
			Piece = GetNextAvailablePieceOnBoard(0, !bIsBlack ? MoveY : -MoveY, LocalMeshSize, PosX, PosY + AChessWarBoard->MeshSize.Y * MoveY);
		}
	}

	return Piece;
}

TArray<FVector2D> AChessWarCharacter::GetAllPossiblePositions()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::SanitizeFloat(Id));
	//UE_LOG(Board, Warning, TEXT("Y = %f"), EachData.Y);
	TPossiblePositions.Empty();
	TDefaultMovements.Empty();

	switch (EPieceType)
	{
	case EPiecesType::E_Bishop:
		SetDiagonalPossiblePositions();
		break;
	case EPiecesType::E_King:
		SetDiagonalPossiblePositions(true);
		SetHorizontalOrVerticalPositions(true);
		SetCastlingPositions();
		break;
	case EPiecesType::E_Knight:
		SetKnightPossiblePositions();
		break;
	case EPiecesType::E_Pawn:
		SetPawnPossiblePositions();
		/*Initialisation d'un array non déclaré*/
		/*FVector2D PossiblePositionsInit[] = { FVector2D(CurrentPiecePosition.X, CurrentPiecePosition.Y + MeshSize.Y), FVector2D(CurrentPiecePosition.X, CurrentPiecePosition.Y + MeshSize.Y * 2) };
		TPossiblePositions.Append(PossiblePositionsInit, UE_ARRAY_COUNT(PossiblePositionsInit));*/
		break;
	case EPiecesType::E_Queen:
		SetDiagonalPossiblePositions();
		SetHorizontalOrVerticalPositions();
		break;
	case EPiecesType::E_Rook:
		SetHorizontalOrVerticalPositions();
		break;
	default:
		break;
	}

	return TPossiblePositions;
}

int32 AChessWarCharacter::GetPiecesNumberBetweenTwoPositionsInDefaultMvt(const FVector2D PosA, const FVector2D PosB) const
{
	auto Counter = 0;

	for (const auto DefaultMovement : TDefaultMovements)
	{
		if (AChessWarBoard->IsPositionBetweenPosAAndPosB(DefaultMovement, PosA, PosB) && !AChessWarBoard->IsEmptyPosition(DefaultMovement))
		{
			Counter++;
		}
	}

	return Counter;
}

void AChessWarCharacter::SetKnightPossiblePositions()
{
	FVector2D PossiblePositions[] = {
		FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * 2, FCurrentPosition.Y + AChessWarBoard->MeshSize.Y),
		FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * 2, FCurrentPosition.Y - AChessWarBoard->MeshSize.Y),
		FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X, FCurrentPosition.Y + AChessWarBoard->MeshSize.Y * 2),
		FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X, FCurrentPosition.Y - AChessWarBoard->MeshSize.Y * 2),
		FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X, FCurrentPosition.Y + AChessWarBoard->MeshSize.Y * 2),
		FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X, FCurrentPosition.Y - AChessWarBoard->MeshSize.Y * 2),
		FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * 2, FCurrentPosition.Y + AChessWarBoard->MeshSize.Y),
		FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * 2, FCurrentPosition.Y - AChessWarBoard->MeshSize.Y)
	};

	TDefaultMovements.Append(PossiblePositions, UE_ARRAY_COUNT(PossiblePositions));

	// Définit les positions possibles avec les possibles prises adverses
	for (const auto EachPossiblePosition : PossiblePositions)
	{
		const auto PieceOnPosition = AChessWarBoard->GetPieceFromPosition(EachPossiblePosition);
		if (AChessWarBoard->IsEmptyPosition(EachPossiblePosition) || PieceOnPosition->IsValidLowLevel() && PieceOnPosition->bIsBlack != bIsBlack)
		{
			SetPossiblePositions(EachPossiblePosition);
		}
		// Définit si la piece en cours protège la pièce en position
		if (!AChessWarBoard->IsEmptyPosition(EachPossiblePosition) && PieceOnPosition->IsValidLowLevel() && PieceOnPosition->bIsBlack == bIsBlack)
		{
			PieceOnPosition->bIsProtected = true;
		}
	}
}

void AChessWarCharacter::SetPawnPossiblePositions()
{
	auto PosY = bIsBlack ? FCurrentPosition.Y + AChessWarBoard->MeshSize.Y : FCurrentPosition.Y - AChessWarBoard->MeshSize.Y;
	const auto PosX = FCurrentPosition.X;
	AChessWarCharacter* PieceOnPosition;

	FVector2D PossiblePositions[] = {FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X, PosY), FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X, PosY)};

	TDefaultMovements.Append(PossiblePositions, UE_ARRAY_COUNT(PossiblePositions));

	// Définit les déplacements si une pièce peut être prise
	for (const auto EachPossiblePosition : PossiblePositions)
	{
		PieceOnPosition = AChessWarBoard->GetPieceFromPosition(EachPossiblePosition);
		if (!AChessWarBoard->IsEmptyPosition(EachPossiblePosition) && PieceOnPosition->IsValidLowLevel() && PieceOnPosition->bIsBlack != bIsBlack)
		{
			SetPossiblePositions(EachPossiblePosition);
		}
		// Définit si la piece en cours protège la pièce en position
		if (!AChessWarBoard->IsEmptyPosition(EachPossiblePosition) && PieceOnPosition->IsValidLowLevel() && PieceOnPosition->bIsBlack == bIsBlack)
		{
			PieceOnPosition->bIsProtected = true;
		}
	}

	// En passant
	FVector2D EnPassantPossiblePositions[] = {FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X, FCurrentPosition.Y), FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X, FCurrentPosition.Y)};

	for (const auto EachPossiblePosition : EnPassantPossiblePositions)
	{
		PieceOnPosition = AChessWarBoard->GetPieceFromPosition(EachPossiblePosition);
		if (PieceOnPosition->IsValidLowLevel() && PieceOnPosition->EPieceType == EPiecesType::E_Pawn && PieceOnPosition->bIsBlack != bIsBlack && PieceOnPosition->bIsTwoSquareFirstMvt)
		{
			AChessWarBoard->PawnToCaptureInEnPassantMovement = PieceOnPosition;
			SetPossiblePositions(FVector2D(EachPossiblePosition.X, bIsBlack ? FCurrentPosition.Y + AChessWarBoard->MeshSize.Y : FCurrentPosition.Y - AChessWarBoard->MeshSize.Y));
			AChessWarBoard->PawnPositionForEnPassantMovement = FVector2D(EachPossiblePosition.X, bIsBlack ? FCurrentPosition.Y + AChessWarBoard->MeshSize.Y : FCurrentPosition.Y - AChessWarBoard->MeshSize.Y);
		}
	}

	// Définit les déplacements par défaut
	for (auto i = 1; i <= 2; i++)
	{
		PosY = bIsBlack ? FCurrentPosition.Y + AChessWarBoard->MeshSize.Y * i : FCurrentPosition.Y - AChessWarBoard->MeshSize.Y * i;

		if (AChessWarBoard->IsEmptyPosition(FVector2D(PosX, PosY)) && (i == 2 ? !bIsMoved : true))
		{
			SetPossiblePositions(FVector2D(PosX, PosY));
		}
		else
		{
			break;
		}
	}

	// Promotion
	if (AChessWarBoard->IsOutBoardOnYPosition(bIsBlack ? FCurrentPosition.Y + AChessWarBoard->MeshSize.Y * 2 : FCurrentPosition.Y - AChessWarBoard->MeshSize.Y * 2))
	{
		AChessWarBoard->PawnToPromote = this;
	}
}

void AChessWarCharacter::SetDiagonalPossiblePositions(const bool bIsOneSquareMovement /*= false*/)
{
	// DefaultMovements
	auto i = 1, j = 1;
	auto IsForwardLeftMovement = true, IsForwardRightMovement = true, IsBackLeftMovement = true, IsBackRightMovement = true;
	bool IsDefaultMovements[] = {IsForwardLeftMovement, IsForwardRightMovement, IsBackLeftMovement, IsBackRightMovement};

	while (IsForwardLeftMovement == true || IsForwardRightMovement == true || IsBackLeftMovement == true || IsBackRightMovement == true)
	{
		// Forward Left Movement, Forward Right Movement, Back Left Movement, Back Right Movement
		FVector2D PossiblePositions[] = {
			FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * i, FCurrentPosition.Y - AChessWarBoard->MeshSize.Y * j),
			FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * i, FCurrentPosition.Y - AChessWarBoard->MeshSize.Y * j),
			FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * i, FCurrentPosition.Y + AChessWarBoard->MeshSize.Y * j),
			FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * i, FCurrentPosition.Y + AChessWarBoard->MeshSize.Y * j)
		};

		for (auto k = 0; k <= 3; k++)
		{
			auto PossiblePosition = PossiblePositions[k];

			if (IsDefaultMovements[k])
			{
				if (!AChessWarBoard->TMeshPositions.Contains(PossiblePosition))
				{
					IsDefaultMovements[k] = false;
				}
			}
		}

		TDefaultMovements.Append(PossiblePositions, UE_ARRAY_COUNT(PossiblePositions));

		IsForwardLeftMovement = bIsOneSquareMovement ? false : IsDefaultMovements[0];
		IsForwardRightMovement = bIsOneSquareMovement ? false : IsDefaultMovements[1];
		IsBackLeftMovement = bIsOneSquareMovement ? false : IsDefaultMovements[2];
		IsBackRightMovement = bIsOneSquareMovement ? false : IsDefaultMovements[3];

		i++, j++;
	}

	// PossiblePositions
	i = 1, j = 1;
	IsForwardLeftMovement = true, IsForwardRightMovement = true, IsBackLeftMovement = true, IsBackRightMovement = true;
	bool IsMovements[] = {IsForwardLeftMovement, IsForwardRightMovement, IsBackLeftMovement, IsBackRightMovement};

	while (IsForwardLeftMovement == true || IsForwardRightMovement == true || IsBackLeftMovement == true || IsBackRightMovement == true)
	{
		// Forward Left Movement, Forward Right Movement, Back Left Movement, Back Right Movement
		FVector2D PossiblePositions[] = {
			FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * i, FCurrentPosition.Y - AChessWarBoard->MeshSize.Y * j),
			FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * i, FCurrentPosition.Y - AChessWarBoard->MeshSize.Y * j),
			FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * i, FCurrentPosition.Y + AChessWarBoard->MeshSize.Y * j),
			FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * i, FCurrentPosition.Y + AChessWarBoard->MeshSize.Y * j)
		};

		for (auto k = 0; k <= 3; k++)
		{
			const auto PossiblePosition = PossiblePositions[k];
			const auto PieceOnPosition = AChessWarBoard->GetPieceFromPosition(PossiblePosition);

			if (IsMovements[k])
			{
				if (AChessWarBoard->IsEmptyPosition(PossiblePosition))
				{
					SetPossiblePositions(PossiblePosition);
				}
				else if (PieceOnPosition->IsValidLowLevel() && PieceOnPosition->bIsBlack != bIsBlack)
				{
					SetPossiblePositions(PossiblePosition);
					IsMovements[k] = false;
				}
				else
				{
					IsMovements[k] = false;
				}
				// Définit si la piece en cours protège la pièce en position
				if (!AChessWarBoard->IsEmptyPosition(PossiblePosition) && PieceOnPosition->IsValidLowLevel() && PieceOnPosition->bIsBlack == bIsBlack)
				{
					PieceOnPosition->bIsProtected = true;
				}
			}
		}

		IsForwardLeftMovement = bIsOneSquareMovement ? false : IsMovements[0];
		IsForwardRightMovement = bIsOneSquareMovement ? false : IsMovements[1];
		IsBackLeftMovement = bIsOneSquareMovement ? false : IsMovements[2];
		IsBackRightMovement = bIsOneSquareMovement ? false : IsMovements[3];

		i++, j++;
	}
}

void AChessWarCharacter::SetHorizontalOrVerticalPositions(const bool bIsOneSquareMovement /*= false*/)
{
	// DefaultMovements
	int32 i = 1;
	bool IsForwardMovement = true, IsBackMovement = true, IsLeftMovement = true, IsRightMovement = true;
	bool IsDefaultMovements[] = {IsForwardMovement, IsBackMovement, IsLeftMovement, IsRightMovement};

	while (IsForwardMovement == true || IsBackMovement == true || IsLeftMovement == true || IsRightMovement == true)
	{
		// Forward Movement, Back Movement, Left Movement, Right Movement
		FVector2D PossiblePositions[] = {
			FVector2D(FCurrentPosition.X, FCurrentPosition.Y - AChessWarBoard->MeshSize.Y * i), FVector2D(FCurrentPosition.X, FCurrentPosition.Y + AChessWarBoard->MeshSize.Y * i),
			FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * i, FCurrentPosition.Y), FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * i, FCurrentPosition.Y)
		};

		for (auto k = 0; k <= 3; k++)
		{
			auto PossiblePosition = PossiblePositions[k];

			if (IsDefaultMovements[k])
			{
				if (!AChessWarBoard->TMeshPositions.Contains(PossiblePosition))
				{
					IsDefaultMovements[k] = false;
				}
			}
		}

		TDefaultMovements.Append(PossiblePositions, UE_ARRAY_COUNT(PossiblePositions));

		IsForwardMovement = bIsOneSquareMovement ? false : IsDefaultMovements[0];
		IsBackMovement = bIsOneSquareMovement ? false : IsDefaultMovements[1];
		IsLeftMovement = bIsOneSquareMovement ? false : IsDefaultMovements[2];
		IsRightMovement = bIsOneSquareMovement ? false : IsDefaultMovements[3];

		i++;
	}

	// PossiblePositions
	i = 1;
	IsForwardMovement = true, IsBackMovement = true, IsLeftMovement = true, IsRightMovement = true;
	bool IsMovements[] = {IsForwardMovement, IsBackMovement, IsLeftMovement, IsRightMovement};

	while (IsForwardMovement == true || IsBackMovement == true || IsLeftMovement == true || IsRightMovement == true)
	{
		// Forward Movement, Back Movement, Left Movement, Right Movement
		FVector2D PossiblePositions[] = {
			FVector2D(FCurrentPosition.X, FCurrentPosition.Y - AChessWarBoard->MeshSize.Y * i), FVector2D(FCurrentPosition.X, FCurrentPosition.Y + AChessWarBoard->MeshSize.Y * i),
			FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * i, FCurrentPosition.Y), FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * i, FCurrentPosition.Y)
		};

		for (auto k = 0; k <= 3; k++)
		{
			const auto PossiblePosition = PossiblePositions[k];
			const auto PieceOnPosition = AChessWarBoard->GetPieceFromPosition(PossiblePosition);

			if (IsMovements[k])
			{
				if (AChessWarBoard->IsEmptyPosition(PossiblePosition))
				{
					SetPossiblePositions(PossiblePosition);
				}
				else if (PieceOnPosition->IsValidLowLevel() && PieceOnPosition->bIsBlack != bIsBlack)
				{
					SetPossiblePositions(PossiblePosition);
					IsMovements[k] = false;
				}
				else
				{
					IsMovements[k] = false;
				}
				// Définit si la piece en cours protège la pièce en position
				if (!AChessWarBoard->IsEmptyPosition(PossiblePosition) && PieceOnPosition->IsValidLowLevel() && PieceOnPosition->bIsBlack == bIsBlack)
				{
					PieceOnPosition->bIsProtected = true;
				}
			}
		}

		IsForwardMovement = bIsOneSquareMovement ? false : IsMovements[0];
		IsBackMovement = bIsOneSquareMovement ? false : IsMovements[1];
		IsLeftMovement = bIsOneSquareMovement ? false : IsMovements[2];
		IsRightMovement = bIsOneSquareMovement ? false : IsMovements[3];

		i++;
	}
}

void AChessWarCharacter::SetCastlingPositions()
{
	if (!bIsMoved)
	{
		if (AChessWarBoard->IsEmptyPosition(FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X, FCurrentPosition.Y)) && AChessWarBoard->
			IsEmptyPosition(FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * 2, FCurrentPosition.Y)) && AChessWarBoard->
			                                                                                                       GetPieceFromPosition(FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * 3, FCurrentPosition.Y))->
			                                                                                                       EPieceType == EPiecesType::E_Rook && !AChessWarBoard->
			GetPieceFromPosition(FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * 3, FCurrentPosition.Y))->bIsMoved && !AChessWarBoard->bCheck && !AChessWarBoard->
			IsCheck(nullptr, FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X, FCurrentPosition.Y)) && !AChessWarBoard->IsCheck(nullptr, FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * 2, FCurrentPosition.Y)))
		{
			SetPossiblePositions(FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * 2, FCurrentPosition.Y));
			AChessWarBoard->RookToMoveInCastlingMovement = AChessWarBoard->GetPieceFromPosition(FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * 3, FCurrentPosition.Y));
			AChessWarBoard->RookPositionsAfterCastlingMovement = FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * 1, FCurrentPosition.Y);
			AChessWarBoard->KingPositionForCastlingMovement = FVector2D(FCurrentPosition.X + AChessWarBoard->MeshSize.X * 2, FCurrentPosition.Y);
		}
		if (AChessWarBoard->IsEmptyPosition(FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X, FCurrentPosition.Y)) && AChessWarBoard->
			IsEmptyPosition(FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * 2, FCurrentPosition.Y)) && AChessWarBoard->IsEmptyPosition(FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * 3, FCurrentPosition.Y))
			&& AChessWarBoard->GetPieceFromPosition(FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * 4, FCurrentPosition.Y))->EPieceType == EPiecesType::E_Rook && !AChessWarBoard->
			GetPieceFromPosition(FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * 4, FCurrentPosition.Y))->bIsMoved && !AChessWarBoard->bCheck && !AChessWarBoard->
			IsCheck(nullptr, FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X, FCurrentPosition.Y)) && !AChessWarBoard->IsCheck(nullptr, FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * 2, FCurrentPosition.Y)))
		{
			SetPossiblePositions(FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * 2, FCurrentPosition.Y));
			AChessWarBoard->RookToMoveInCastlingMovement = AChessWarBoard->GetPieceFromPosition(FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * 4, FCurrentPosition.Y));
			AChessWarBoard->RookPositionsAfterCastlingMovement = FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * 1, FCurrentPosition.Y);
			AChessWarBoard->KingPositionForCastlingMovement = FVector2D(FCurrentPosition.X - AChessWarBoard->MeshSize.X * 2, FCurrentPosition.Y);
		}
	}
}

void AChessWarCharacter::SetPossiblePositions(const FVector2D PossiblePosition)
{
	if (!AChessWarBoard->IsCheck(this, PossiblePosition))
	{
		TPossiblePositions.Add(PossiblePosition);
	}
}

// Called when the game starts or when spawned
/*
void AChessWarCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
*/

// Called every frame
/*
void AChessWarCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
*/

// Called to bind functionality to input
/*
void AChessWarCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
*/
