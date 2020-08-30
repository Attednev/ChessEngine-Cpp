#include <iostream>

using namespace std;



class LogEntry {
	
};


class Vector {
	public: int x;
	public: int y;
	public: Vecot(int x, int y) {
		this->x = x;
		this->y = y;
	}
};


class GameState {
	// This vairalbe stores the current board, the pieces are expressed as a combination of two letters,
	// The first letter defines the color; the second the piece type. Empty squares are defined as "  "
	public: string board[8][8] = {
		{"bR", "bN", "bB", "bQ", "bK", "bB", "bN", "bR"},
		{"bP", "bP", "bP", "bP", "bP", "bP", "bP", "bP"},
		{"  ", "  ", "  ", "  ", "  ", "  ", "  ", "  "},
		{"  ", "  ", "  ", "  ", "  ", "  ", "  ", "  "},
		{"  ", "  ", "  ", "  ", "  ", "  ", "  ", "  "},
		{"  ", "  ", "  ", "  ", "  ", "  ", "  ", "  "},
		{"wP", "wP", "wP", "wP", "wP", "wP", "wP", "wP"},
		{"wR", "wN", "wB", "wQ", "wK", "wB", "wN", "wR"}
	};
	private: bool isWhiteToMove = true;
	private: list<vector<int>, vector<int>>
	
	// This function is used for an external program which uses this engine to pass in a move a player makes
	public: void playMove(int startRow, int startCol, int endRow, int endCol) {
		// is the move allowd
		// play the move
		// log the board
		// chage turn
		// get moves for the next player
	}
	
	// this function is used to undo the last recent move played
	public: void undoMove() {
		// get last move
		// restore the board
		// get moves for the current player
	}
	
	
	
	
	
};



