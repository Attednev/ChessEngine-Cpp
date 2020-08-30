#include <iostream>
#include <array>
#include <list>
using namespace std;

// NOTE: Casteling
// NOTE: Add en-passant
// NOTE: Add pawn promotion

class LogEntry {
	public: string board[8][8];
	public: bool isWhiteToMove;
	public: LogEntry(string board[8][8], bool isWhiteToMove) {
		copy(&board[0][0], &board[0][0] + 64, &this->board[0][0]);
		this->isWhiteToMove = isWhiteToMove;
	}
};

// Helper class to store a x and y value
class Vector {
	public: int x;
	public: int y;
	public: Vector(int x, int y) {
		this->x = x;
		this->y = y;
	}
	public: equals(Vector v) { return (this->x == v.x && this->y == v.y); }
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
	private: list<array<Vector, 2>> validMoves;
	private: list<LogEntry> log;
	
	// Costructor: calls the getValidMoves function so the white player can play a move
	public: GameState() { this->getValidMoves(); }
	
	// This function is used for an external program which uses this engine to pass in a move a player makes
	public: void playMove(int startRow, int startCol, int endRow, int endCol) {
		// is the move allowd
		Vector move[2] = {{startRow, startCol}, {endRow, endCol}};
		for (array<Vector,2> arr : this->validMoves) {
			if (arr[0].equals(move[0]) && arr[1].equals(move[1])) {
				// play the move
				this->board[endRow][endCol] = this->board[startRow][startCol];
				this->board[startRow][startCol] = "  ";
				// log the board
				LogEntry entry(this->board, this->isWhiteToMove);
				this->log.push_back(entry);
				this->isWhiteToMove = !this->isWhiteToMove; // chage turn
				this->getValidMoves(); // get moves for the next player
			}
		}
	}
	
	// this function is used to undo the last recent move played
	public: void undoMove() {
		// get last move
		LogEntry entry = this->log.back();
		this->log.pop_back();
		// restore the gamestate
		copy(&this->board[0][0], &this->board[0][0] + 64, &entry.board[0][0]);
		this->isWhiteToMove = entry.isWhiteToMove;
		// get moves for the current player
		this->getValidMoves();
	}
	
	// This function fills the "validMoves" variable with all the possible moves that the current player can play
	private: void getValidMoves() {
		list<array<Vector, 2>> list;
		this->getAllMoves(list); // get all moves
		this->removeIllegalMoves(list); // remove the invalid moves
		this->validMoves = list;
	}
	
	// Helper function to fill the "validMoves" variable with all moves (also illegal moves) that the user has available
	private: void getAllMoves(list<array<Vector, 2>>& list) {
		char turn = this->isWhiteToMove ? 'w' : 'b';
		// Loop through all fields and check if there is piece that the current player owns and get its moves
		for (int r = 0; r < 8; r++) {
			for (int c = 0; c < 8; c++) {
				if ((this->board[r][c]).at(0) == turn) {
					char piece = (this->board[r][c]).at(1);
					switch (piece) {
						case 'P':
							this->addPawnMove(r, c, list);
						break;
						case 'R':
							this->addRookMove(r, c, list);
						break;
						case 'N':
							this->addKnightMove(r, c, list);
						break;
						case 'B':
							this->addBishopMove(r, c, list);
						break;
						case 'Q':
							this->addQueenMove(r, c, list);
						break;
						default:
							this->addKingMove(r, c, list);
					}
				}
			}
		}
	}
	
	// Helper function to remove all illegal moves from the list
	private: void removeIllegalMoves(list<array<Vector, 2>>& list) {
		
	}
	
	// Function to add all the possible moves for pawns
	private: void addPawnMove(int r, int c, list<array<Vector, 2>>& list) {
		int offset = this->isWhiteToMove ? -1 : 1;
		if (this->board[r + offset][c] == "  ") { // Single pawn advance (Check field infront of the pawn)
			list.push_back(array<Vector, 2>{*new Vector(r, c), *new Vector(r + offset, c)});
			if (r == 6 && this->board[r + (2 * offset)][c] == "  ") { // Double pawn advance (Check the field 2 infront of the pawn)
				list.push_back(array<Vector, 2>{*new Vector(r, c), *new Vector(r + (2 * offset), c)});
			}
		}
		char enemy = this->isWhiteToMove ? 'b' : 'w';
		if (c <= 7 && (this->board[r + offset][c + 1]).at(0) == enemy) { // Capture to the right
			list.push_back(array<Vector, 2>{*new Vector(r, c), *new Vector(r + offset, c + 1)});
		}
		if (c >= 0 && (this->board[r + offset][c - 1]).at(0) == enemy) { // Capture to the left
			list.push_back(array<Vector, 2>{*new Vector(r, c), *new Vector(r + offset, c - 1)});
		}
	}
	
	// Function to add all the possible moves for rooks
	private: void addRookMove(int r, int c, list<array<Vector, 2>>& list) {
		int rows[4] = {-1, 1, 0, 0};
		int cols[4] = {0, 0, 1, -1};
		this->bishopRookHelper(rows, cols, r, c, list);
	}
	
	// Function to add all the possible moves for bishops
	private: void addBishopMove(int r, int c, list<array<Vector, 2>>& list) {
		int rows[4] = {-1, 1, -1, 1};
		int cols[4] = {-1, 1, 1, -1};
		this->bishopRookHelper(rows, cols, r, c, list);
	}
	
	// Helper function for rooks and bishops
	private: void bishopRookHelper(int rows[], int cols[], int r, int c, list<array<Vector, 2>>& list) {
		char turn = this->isWhiteToMove ? 'w' : 'b';
		char enemy = this->isWhiteToMove ? 'b' : 'w';
		for (int i = 0; i < 4; i++) {
			for (int row = r + rows[i], col = c + cols[i]; row < 8 && row >= 0 && col < 8 && col >= 0; row += rows[i], col += cols[i]) {
				if ((this->board[row][col]).at(0) == turn) break; // Own Piece, so no need to keep on checkig
				list.push_back(array<Vector, 2>{*new Vector(r, c), *new Vector(row, col)});
				if ((this->board[row][col]).at(0) == enemy) break; // Enemy Piece, so allow to capture, but dont go further
			}
		}
	}
	
	// Function to add all the possible moves for knights
	private: void addKnightMove(int r, int c, list<array<Vector, 2>>& list) {
		char turn = this->isWhiteToMove ? 'w' : 'b';
		int rows[8] = {-2, -2, 2, 2, -1, 1, -1, 1};
		int cols[8] = {-1, 1, -1, 1, -2, -2, 2, 2};
		for (int i = 0; i < 8; i++) {
			int row = r + rows[i];
			int col = c + cols[i];
			// Inside the board and not attacking an ally
			if (row >= 0 && row < 8 && col >= 0 && col < 8 && (this->board[row][col]).at(0) != turn) {
				list.push_back(array<Vector, 2>{*new Vector(r, c), *new Vector(row, col)});
			}
		}
	}
	
	// Function to add all the possible moves for the queen
	private: void addQueenMove(int r, int c, list<array<Vector, 2>>& list) {
		this->addRookMove(r, c, list);
		this->addBishopMove(r, c, list);
	}
	
	// Function to add all the possible moves for the king
	private: void addKingMove(int r, int c, list<array<Vector, 2>>& list) {
		char turn = this->isWhiteToMove ? 'w' : 'b';
		int rows[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
		int cols[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
		for (int i = 0; i < 8; i++) {
			int row = r + rows[i];
			int col = c + cols[i];
			if (row >= 0 && row < 8 && col >= 0 && col < 8 && (this->board[row][col]).at(0) != turn) {
				list.push_back(array<Vector, 2>{*new Vector(r, c), *new Vector(row, col)});
			}
		}
	}	
	
};




















