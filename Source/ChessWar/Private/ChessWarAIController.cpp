// Copyright 2020 Nicolas Boyer. All Rights Reserved.


#include "ChessWarAIController.h"

float AChessWarAIController::GetEvaluationByPieceType(const EPiecesType EPieceType, const bool IsBlack, const int X, const int Y)
{
	float PawnEvalWhite[8][8] = {
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0},
		{1.0, 1.0, 2.0, 3.0, 3.0, 2.0, 1.0, 1.0},
		{0.5, 0.5, 1.0, 2.5, 2.5, 1.0, 0.5, 0.5},
		{0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0},
		{0.5, -0.5, -1.0, 0.0, 0.0, -1.0, -0.5, 0.5},
		{0.5, 1.0, 1.0, -2.0, -2.0, 1.0, 1.0, 0.5},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
	};

	float PawnEvalBlack[8][8] = {
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.5, 1.0, 1.0, -2.0, -2.0, 1.0, 1.0, 0.5},
		{0.5, -0.5, -1.0, 0.0, 0.0, -1.0, -0.5, 0.5},
		{0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0},
		{0.5, 0.5, 1.0, 2.5, 2.5, 1.0, 0.5, 0.5},
		{1.0, 1.0, 2.0, 3.0, 3.0, 2.0, 1.0, 1.0},
		{5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
	};

	float KnightEval[8][8] = {
		{-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0},
		{-4.0, -2.0, 0.0, 0.0, 0.0, 0.0, -2.0, -4.0},
		{-3.0, 0.0, 1.0, 1.5, 1.5, 1.0, 0.0, -3.0},
		{-3.0, 0.5, 1.5, 2.0, 2.0, 1.5, 0.5, -3.0},
		{-3.0, 0.0, 1.5, 2.0, 2.0, 1.5, 0.0, -3.0},
		{-3.0, 0.5, 1.0, 1.5, 1.5, 1.0, 0.5, -3.0},
		{-4.0, -2.0, 0.0, 0.5, 0.5, 0.0, -2.0, -4.0},
		{-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0}
	};

	float BishopEvalWhite[8][8] = {
		{-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0},
		{-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0},
		{-1.0, 0.0, 0.5, 1.0, 1.0, 0.5, 0.0, -1.0},
		{-1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5, -1.0},
		{-1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, -1.0},
		{-1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0},
		{-1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, -1.0},
		{-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0}
	};

	float BishopEvalBlack[8][8] = {
		{-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0},
		{-1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, -1.0},
		{-1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0},
		{-1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, -1.0},
		{-1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5, -1.0},
		{-1.0, 0.0, 0.5, 1.0, 1.0, 0.5, 0.0, -1.0},
		{-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0},
		{-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0}
	};

	float RookEvalWhite[8][8] = {
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.5},
		{-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
		{-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
		{-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
		{-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
		{-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
		{0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0}
	};

	float RookEvalBlack[8][8] = {
		{0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0},
		{-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
		{-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
		{-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
		{-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
		{-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
		{0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.5},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
	};

	float QueenEval[8][8] = {
		{-2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0},
		{-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0},
		{-1.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -1.0},
		{-0.5, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -0.5},
		{0.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -0.5},
		{-1.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.5, -1.0},
		{-1.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, -1.0},
		{-2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0}
	};

	float KingEvalWhite[8][8] = {
		{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
		{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
		{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
		{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
		{-2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0},
		{-1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0},
		{2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0},
		{2.0, 3.0, 1.0, 0.0, 0.0, 1.0, 3.0, 2.0}
	};

	float KingEvalBlack[8][8] = {
		{2.0, 3.0, 1.0, 0.0, 0.0, 1.0, 3.0, 2.0},
		{2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0},
		{-1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0},
		{-2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0},
		{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
		{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
		{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
		{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0}
	};

	switch (EPieceType)
	{
	case EPiecesType::E_Bishop: return IsBlack ? BishopEvalBlack[X][Y] : BishopEvalWhite[X][Y];
	case EPiecesType::E_King: return IsBlack ? KingEvalBlack[X][Y] : KingEvalWhite[X][Y];
	case EPiecesType::E_Knight: return KnightEval[X][Y];
	case EPiecesType::E_Pawn: return IsBlack ? PawnEvalBlack[X][Y] : PawnEvalWhite[X][Y];
	case EPiecesType::E_Queen: return QueenEval[X][Y];
	case EPiecesType::E_Rook: return IsBlack ? RookEvalBlack[X][Y] : RookEvalWhite[X][Y];
	}


	return 0;
}
