#include <iostream>
#include <array>
#include <list>

using namespace std;



class LogEntry {
	
};


class Vector {
	public: int x;
	public: int y;
	public: Vector(int x, int y) {
		this->x = x;
		this->y = y;
	}
	public: equals(Vector v) { return (this->x == v.x && this->y == v.y); }
};



		// NOTE: Add en-passant
		// NOTE: Add pawn promotion

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
	private: list<array<Vector, 2>> validMoves;
	private: list<LogEntry> log;
	
	// This function is used for an external program which uses this engine to pass in a move a player makes
	public: void playMove(int startRow, int startCol, int endRow, int endCol) {
		// is the move allowd
		//Vector move[2] = {{startRow, startCol}, {endRow, endCol}};
		//for (array<Vector,2> arr : this->validMoves) {
			//if (arr[0].equals(move[0]) && arr[1].equals(move[1])) {
				// play the move
				this->board[endRow][endCol] = this->board[startRow][startCol];
				this->board[startRow][startCol] = "  ";
				// log the board
				this->isWhiteToMove = !this->isWhiteToMove; // chage turn
				// get moves for the next player
				this->getValidMoves();
			//}
		//}
	}
	
	// this function is used to undo the last recent move played
	public: void undoMove() {
		// get last move
		// restore the board
		// get moves for the current player
	}
	
	// This function fills the "validMoves" variable with all the possible moves that the current player can play
	private: void getValidMoves() {
		// clear the current list
		// get all moves
	}
	
	// Helper function to fill the "validMoves" variable with all moves (also illegal moves) that the user has available
	private: void getAllMoves() {
		char turn = this->isWhiteToMove ? 'w' : 'b';
		for (int r = 0; r < 8; r++) {
			for (int c = 0; c < 8; c++) {
				if ((this->board[r][c]).at(0) == turn) {
					char piece = (this->board[r][c]).at(1);
					switch (piece) {
						case 'P':
							this->addPawnMove(r, c);
						break;
						case 'R':
							this->addRookMove(r, c);
						break;
						case 'N':
							this->addKnightMove(r, c);
						break;
						case 'B':
							this->addBishopMove(r, c);
						break;
						case 'Q':
							this->addQueenMove(r, c);
						break;
						default:
							this->addKingMove(r, c);
					}
				}
			}
		}
	}
	
	// Function to add all the possible moves for pawns
	private: void addPawnMove(int r, int c) {
		int offset = this->isWhiteToMove ? -1 : 1;
		if (this->board[r + offset][c] == "  ") { // Single pawn advance (Check field infront of the pawn)
			this->validMoves.push_back(*new Move(r, c, r + offset, c, this->board));
			if (r == 6 && this->board[r + (2 * offset)][c] == "  ") { // Double pawn advance (Check the field 2 infront of the pawn)
				this->validMoves.push_back(*new Move(r, c, r + (2 * offset), c, this->board));
			}
		}
		char enemy = this->isWhiteToMove ? 'b' : 'w';
		if (c <= 7 && (this->board[r + offset][c + 1]).at(0) == enemy) { // Capture to the right
			this->validMoves.push_back(*new Move(r, c, r + offset, c + 1, this->board));
		}
		if (c >= 0 && (this->board[r + offset][c - 1]).at(0) == enemy) { // Capture to the left
			this->validMoves.push_back(*new Move(r, c, r + offset, c - 1, this->board));
		}
	}
	
	// Function to add all the possible moves for rooks
	private: void addRookMove(int r, int c) {
		int rows[4] = {-1, 1, 0, 0};
		int cols[4] = {0, 0, 1, -1};
		this->bishopRookHelper(rows, cols, r, c);
	}
	
	// Function to add all the possible moves for bishops
	private: void addBishopMove(int r, int c) {
		int rows[4] = {-1, 1, -1, 1};
		int cols[4] = {-1, 1, 1, -1};
		this->bishopRookHelper(rows, cols, r, c);
	}
	
	// Helper function for rooks and bishops
	private: void bishopRookHelper(int rows[], int cols[], int r, int c) {
		char turn = this->isWhiteToMove ? 'w' : 'b';
		char enemy = this->isWhiteToMove ? 'b' : 'w';
		for (int i = 0; i < 4; i++) {
			for (int row = r + rows[i], col = c + cols[i]; row < 8 && row >= 0 && col < 8 && col >= 0; row += rows[i], col += cols[i]) {
				if ((this->board[row][col]).at(0) == turn) break; // Own Piece, so no need to keep on checkig
				this->validMoves.push_back(*new Move(r, c, row, col, this->board));
				if ((this->board[row][col]).at(0) == enemy) break; // Enemy Piece, so allow to capture, but dont go further
			}
		}
	}
	
	// Function to add all the possible moves for knights
	private: void addKnightMove(int r, int c) {
		char turn = this->isWhiteToMove ? 'w' : 'b';
		int rows[8] = {-2, -2, 2, 2, -1, 1, -1, 1};
		int cols[8] = {-1, 1, -1, 1, -2, -2, 2, 2};
		for (int i = 0; i < 8; i++) {
			int row = r + rows[i];
			int col = c + cols[i];
			// Inside the board and not attacking an ally
			if (row >= 0 && row < 8 && col >= 0 && col < 8 && (this->board[row][col]).at(0) != turn) {
				this->validMoves.push_back(*new Move(r, c, row, col, this->board));
			}
		}
	}
	
	// Function to add all the possible moves for the queen
	private: void addQueenMove(int r, int c) {
		this->addRookMove(r, c);
		this->addBishopMove(r, c);
	}
	
	// Function to add all the possible moves for the king
	private: void addKingMove(int r, int c) {
		char turn = this->isWhiteToMove ? 'w' : 'b';
		int rows[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
		int cols[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
		for (int i = 0; i < 8; i++) {
			int row = r + rows[i];
			int col = c + cols[i];
			if (row >= 0 && row < 8 && col >= 0 && col < 8 && (this->board[row][col]).at(0) != turn) {
				this->validMoves.push_back(*new Move(r, c, row, col, this->board));
			}
		}
	}
	
	
	
};




















