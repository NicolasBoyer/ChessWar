// Fill out your copyright notice in the Description page of Project Settings.


// #include "ChessWarCharacter.h"
#include "Board.h"

DEFINE_LOG_CATEGORY(GBoard);

// Autres pièces animées
// IA
// Poser un log dans les librairies

// Sets default values
ABoard::ABoard(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), PieceToCapture(nullptr),
                                                              RookToMoveInCastlingMovement(nullptr),
                                                              PawnToCaptureInEnPassantMovement(nullptr),
                                                              PawnToPromote(nullptr),
                                                              bCheck(false),
                                                              ACurrentKing(nullptr)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	//bIsCheckTest = false;
}

bool ABoard::IsPossibleMovementFromTarget(TArray<FVector2D> PossiblePositions, const float TargetX, const float TargetY, FVector2D& NewRookPositions, AChessWarCharacter*& RookForCastlingMvt, bool& IsPawnPromotion)
{
	for (const auto& EachPossiblePosition : PossiblePositions)
	{
		if (EachPossiblePosition.X == TargetX && EachPossiblePosition.Y == TargetY)
		{
			// Pawn Promotion
			IsPawnPromotion = PawnToPromote && PawnToPromote->bIsBlack == ACurrentKing->bIsBlack;
			// Castling Movement
			RookForCastlingMvt = nullptr;
			if (KingPositionForCastlingMovement.X == TargetX && KingPositionForCastlingMovement.Y == TargetY)
			{
				RookForCastlingMvt = RookToMoveInCastlingMovement;
				NewRookPositions = RookPositionsAfterCastlingMovement;
			}
			// Mouvement en passant ou classique avec prise de pièce
			PieceToCapture = PawnPositionForEnPassantMovement.X == TargetX && PawnPositionForEnPassantMovement.Y == TargetY ? PawnToCaptureInEnPassantMovement : GetPieceFromPosition(EachPossiblePosition);
			return true;
		}
	}
	return false;
}

bool ABoard::IsEmptyPosition(const FVector2D Position) const
{
	if (!TMeshPositions.Contains(Position))
	{
		return false;
	}
	if (GetPieceFromPosition(Position)->IsValidLowLevel())
	{
		return false;
	}
	return true;
}

AChessWarCharacter* ABoard::GetPieceToCaptureOnTarget() const
{
	return PieceToCapture;
}

void ABoard::GetCheckState(AChessWarCharacter* CurrentKing, bool& bIsCheckMate, bool& bIsStaleMate, bool& bIsCheck)
{
	if (CurrentKing->IsValidLowLevel())
	{
		bCheck = false, bIsCheckMate = true, bIsStaleMate = true;
		ACurrentKing = CurrentKing;
		KingPositionForCastlingMovement = FVector2D(0.f, 0.f);
		PawnPositionForEnPassantMovement = FVector2D(0.f, 0.f);
		PawnToPromote = nullptr;

		// On test les pièces adverses pour connaître leurs positions et pour savoir s'il y a un échec
		for (auto EachPiece : TPieces)
		{
			if (EachPiece->bIsBlack != CurrentKing->bIsBlack)
			{
				EachPiece->TPossiblePositions = EachPiece->GetAllPossiblePositions();

				for (const auto EachPossiblePosition : EachPiece->TPossiblePositions)
				{
					const auto PieceOnPosition = GetPieceFromPosition(EachPossiblePosition);
					if (PieceOnPosition->IsValidLowLevel() && PieceOnPosition == ACurrentKing)
					{
						EachPiece->bHasChecked = true;
						bCheck = true;
					}
				}
			}
		}

		// On test les pièces en cours pour connaître leurs positions et pour savoir s'il y a échec et mate ou échec et pat
		for (auto EachPiece : TPieces)
		{
			if (EachPiece->bIsBlack == CurrentKing->bIsBlack)
			{
				EachPiece->TPossiblePositions = EachPiece->GetAllPossiblePositions();

				// Checkmate + stalemate
				if (EachPiece->TPossiblePositions.Num() > 0)
				{
					bIsCheckMate = false;
					bIsStaleMate = false;
				}
			}
		}

		bIsCheck = bCheck, bIsCheckMate = bCheck ? bIsCheckMate : false;
	}
}

AChessWarCharacter* ABoard::GetPieceFromPosition(const FVector2D Position) const
{
	for (auto EachPiece : TPieces)
	{
		if (EachPiece->FCurrentPosition.X == Position.X && EachPiece->FCurrentPosition.Y == Position.Y)
		{
			return EachPiece;
		}
	}
	return nullptr;
}

bool ABoard::IsCheck(AChessWarCharacter* SelectedPiece, const FVector2D PossiblePosition)
{
	if (!SelectedPiece || SelectedPiece->bIsBlack == ACurrentKing->bIsBlack)
	{
		const auto CurrentKingPosition = ACurrentKing->FCurrentPosition;
		const auto CurrentPiece = !SelectedPiece ? ACurrentKing : SelectedPiece;

		for (auto EachPiece : TPieces)
		{
			if (EachPiece->bIsBlack != ACurrentKing->bIsBlack)
			{
				// Test si en échec - le roi est en échec
				if (EachPiece->TPossiblePositions.Contains(CurrentKingPosition))
				{
					if (EachPiece->TPossiblePositions.Contains(PossiblePosition) && IsPositionBetweenPosAAndPosB(PossiblePosition, EachPiece->FCurrentPosition, CurrentKingPosition) && !CurrentPiece->bIsProtectingKing)
					{
						return CurrentPiece == ACurrentKing ? true : false;
					}
					if (PossiblePosition.X == EachPiece->FCurrentPosition.X && PossiblePosition.Y == EachPiece->FCurrentPosition.Y && CurrentPiece != ACurrentKing)
					{
						return false;
					}
					if (CurrentPiece != ACurrentKing)
					{
						return true;
					}
				}

				// Test le nombre de pieces entre le roi et la pièce itérée
				if (EachPiece->TDefaultMovements.Contains(CurrentKingPosition) && EachPiece->TDefaultMovements.Contains(CurrentPiece->FCurrentPosition) && CurrentPiece != ACurrentKing && EachPiece->
					GetPiecesNumberBetweenTwoPositionsInDefaultMvt(EachPiece->FCurrentPosition, CurrentKingPosition) > 1)
				{
					CurrentPiece->bIsProtectingKing = false;
					return false;
				}

				// Si la piece courante n'est pas le roi et entre la piece itérée et le roi ou si la piece courante est le roi et que la pièce itéré contient la position et n'est pas un pawn ou est un pawn et ne contient pas la possible position ou si la piece itérée est protégée et est sur une possible position du roi, on bloc cette position
				if (EachPiece->TDefaultMovements.Contains(CurrentKingPosition) && EachPiece->TPossiblePositions.Contains(CurrentPiece->FCurrentPosition) && CurrentPiece != ACurrentKing &&
					IsPositionBetweenPosAAndPosB(CurrentPiece->FCurrentPosition, EachPiece->FCurrentPosition, CurrentKingPosition) || CurrentPiece == ACurrentKing && (EachPiece->TPossiblePositions.Contains(PossiblePosition) &&
						EachPiece->EPieceType != EPiecesType::E_Pawn || EachPiece->EPieceType == EPiecesType::E_Pawn && !EachPiece->TPossiblePositions.Contains(PossiblePosition) && EachPiece->TDefaultMovements.
						Contains(PossiblePosition)) || CurrentPiece == ACurrentKing && EachPiece->bIsProtected && EachPiece->FCurrentPosition == PossiblePosition)
				{
					// Renvoie un check pour le roi
					if (CurrentPiece == ACurrentKing || bCheck)
					{
						return true;
					}

					// Si la pièce courante n'est pas le rois, elle protège le roi
					if (CurrentPiece != ACurrentKing)
					{
						CurrentPiece->bIsProtectingKing = true;
					}

					// Pour chaque défaut movement, si le défaut movement est égale à la position possible et est entre le roi et la pièce itérée
					for (const auto DefaultMovement : EachPiece->TDefaultMovements)
					{
						if (DefaultMovement == PossiblePosition && IsPositionBetweenPosAAndPosB(PossiblePosition, EachPiece->FCurrentPosition, CurrentKingPosition))
						{
							return false;
						}
					}

					// Si la position possible est égale la position courante de la pièce itérée
					if (PossiblePosition == EachPiece->FCurrentPosition)
					{
						return false;
					}

					return true;
				}

				// Si le roi est la pièce courante et qu'il est en échec, il ne peut pas se déplacer dans les mouvements par défaut de la pièce qui le met en échec
				if (CurrentPiece == ACurrentKing && EachPiece->TDefaultMovements.Contains(PossiblePosition) && EachPiece->bHasChecked)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool ABoard::IsOutBoardOnYPosition(const float PosY) const
{
	for (const auto EachMeshPosition : TMeshPositions)
	{
		if (EachMeshPosition.Y == PosY)
		{
			return false;
		}
	}
	return true;
}

bool ABoard::IsPositionBetweenPosAAndPosB(const FVector2D Position, const FVector2D PosA, const FVector2D PosB, const bool IncludeA /*= false*/, const bool IncludeB /*= false*/)
{
	const auto MaxValueX = PosA.X > PosB.X ? PosA.X : PosB.X;
	const auto MinValueX = PosA.X < PosB.X ? PosA.X : PosB.X;
	const auto MaxValueY = PosA.Y > PosB.Y ? PosA.Y : PosB.Y;
	const auto MinValueY = PosA.Y < PosB.Y ? PosA.Y : PosB.Y;

	if ((Position.X > MinValueX && Position.X < MaxValueX && Position.Y > MinValueY && Position.Y < MaxValueY || MinValueX == MaxValueX && MinValueX == Position.X && Position.Y > MinValueY && Position.Y < MaxValueY || Position.X >
		MinValueX && Position.X < MaxValueX && MinValueY == MaxValueY && MinValueY == Position.Y) || (IncludeA && Position == PosA) || (IncludeB && Position == PosB))
	{
		return true;
	}
	return false;
}

// Called when the game starts or when spawned
/*
void ABoard::BeginPlay()
{
	Super::BeginPlay();
	
}
*/

// Called every frame
/*
void ABoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
*/
