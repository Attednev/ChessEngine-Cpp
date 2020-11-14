#include "GameState.h"

GameState::GameState() {
    this->getValidMoves();
    LogEntry entry(this->board, this->isWhiteToMove, this->kingPositions, this->castleRights);
    this->log.push_back(entry);
}

GameState::~GameState() {
    delete promotionPos;
}

std::list <Vector> GameState::getMovesForPiece(int r, int c) {
    // Get all the possible moves (same as in the normal process for validating the moves)
    char turn = this->isWhiteToMove ? 'w' : 'b';
    std::list <std::array<Vector, 2>> moves;
    if (this->board[r][c].at(0) == turn) {
        char piece = this->board[r][c].at(1);
        switch (piece) {
            case 'P':
                this->addPawnMove(r, c, moves);
                break;
            case 'R':
                this->addRookMove(r, c, moves);
                break;
            case 'N':
                this->addKnightMove(r, c, moves);
                break;
            case 'B':
                this->addBishopMove(r, c, moves);
                break;
            case 'Q':
                this->addQueenMove(r, c, moves);
                break;
            default:
                this->addKingMove(r, c, moves);
        }
        this->removeIllegalMoves(moves);
    }
    // Remove the source position since it is not needed
    std::list <Vector> returnVal;
    for (std::array<Vector, 2> m : moves) {
        returnVal.push_back(m[1]);
    }
    return returnVal;
}

int GameState::playMove(int startRow, int startCol, int endRow, int endCol) {
    Vector move[2] = {{startRow, startCol},
                      {endRow,   endCol}};
    for (std::array<Vector, 2> arr : this->validMoves) {
        if (arr[0].equals(move[0]) && arr[1].equals(move[1])) {
            // The move is allowed
            this->sideEffects(startRow, startCol, endRow, endCol);
            this->move(startRow, startCol, endRow, endCol);
            // Log the board
            LogEntry entry(this->board, this->isWhiteToMove, this->kingPositions, this->castleRights);
            this->log.push_back(entry);
            // Check for insufficient material
            if (this->checkInsufficientMaterial()) return 2; // Stalemate due to insufficient material
            this->isWhiteToMove = !this->isWhiteToMove; // change turn
            this->getValidMoves(); // Get moves for the next player
            if (this->validMoves.empty()) {
                if (this->isCheck()) {
                    return this->isWhiteToMove ? 5
                                               : 6; // Return 5 or 6 because someone has lost (5: White lost, 6: Black lost)
                }
                return 2; // Return 2 because it is a stalemate
            } else if (this->isCheck()) {
                return this->isWhiteToMove ? 3
                                           : 4; // Return 3 or 4 because it is check (3: White in check, 4: Black in check)
            }
            return this->inPromotion ? 1
                                     : 0; // Return 1 if the move results in a promotion, 0 if it is a normal move
        }
    }
    return -1; // Return -1 if no move could be executed
}

void GameState::sideEffects(int startRow, int startCol, int endRow, int endCol) {
    if (this->board[startRow][startCol].at(1) == 'K') {
        // Take away the castle right for this player as the king move
        this->castleRights.at(this->isWhiteToMove ? 'w' : 'b')[0] = false;
        this->castleRights.at(this->isWhiteToMove ? 'w' : 'b')[1] = false;
        if (startCol - endCol == 2) { // Castled long
            this->move(startRow, 0, startRow, 3);
        } else if (startCol - endCol == -2) { // Castled short
            this->move(startRow, 7, startRow, 5);
        }
    } else if (this->board[startRow][startCol].at(1) == 'P') {
        if (startCol != endCol && this->board[endRow][endCol].at(1) == ' ') {
            // Captured en passant -> clear the square behind the current one
            this->board[startRow][endCol] = "  ";
        }
        if (endRow == 0 || endRow == 7) {
            // Promotion
            this->inPromotion = true;
            this->promotionPos = new Vector(endRow, endCol);
        }
    } else if (this->board[startRow][startCol].at(1) == 'R') {
        if (startCol == 0) { // Take away right to castle long
            this->castleRights.at(this->isWhiteToMove ? 'w' : 'b')[0] = false;
        } else if (startCol == 7) { // Take away right to castle short
            this->castleRights.at(this->isWhiteToMove ? 'w' : 'b')[1] = false;
        }
    }
}

bool GameState::checkInsufficientMaterial() {
    // Store all pieces into lists
    std::list<char> whitePiecesLeft;
    std::list<char> blackPiecesLeft;
    for (auto &i : this->board) {
        for (auto &j : i) {
            if (j.at(0) == 'w') {
                whitePiecesLeft.push_back(j.at(1));
            } else if (j.at(0) == 'b') {
                blackPiecesLeft.push_back(j.at(1));
            }
        }
    }
    return GameState::checkIfListIsInStaleMate(whitePiecesLeft) &&
           GameState::checkIfListIsInStaleMate(blackPiecesLeft);
}

bool GameState::checkIfListIsInStaleMate(std::list<char> pieces) {
    pieces.sort();
    if (pieces.size() == 1) return true; // Lone King
    if (pieces.size() == 2)
        return pieces.front() == 'B' || pieces.back() == 'N'; // King and Bishop or King and Knight
    if (pieces.size() == 3) return pieces.back() == 'N' && *next(pieces.begin(), 1) == 'N'; // King and two Knights
    return false;
}

void GameState::move(int startRow, int startCol, int endRow, int endCol) {
    // Update King position before executing the move
    if (this->board[startRow][startCol].at(1) == 'K') {
        this->kingPositions.at(this->board[startRow][startCol].at(0)).x = endRow;
        this->kingPositions.at(this->board[startRow][startCol].at(0)).y = endCol;
    }
    // Play the move
    this->board[endRow][endCol] = this->board[startRow][startCol];
    this->board[startRow][startCol] = "  ";
}

void GameState::undoMove() {
    // get last move
    LogEntry entry = this->log.back();
    this->log.pop_back();
    // restore the GameState
    copy(&this->board[0][0], &this->board[0][0] + 64, &entry.board[0][0]);
    this->isWhiteToMove = entry.isWhiteToMove;
    this->kingPositions = entry.kingPositions;
    this->castleRights = entry.castleRights;
    this->getValidMoves(); // get moves for the current player
}

int GameState::promote(int piece) {
    if (this->inPromotion) {
        char promoteTo;
        switch (piece) {
            case 1:
                promoteTo = 'Q';
                break;
            case 2:
                promoteTo = 'N';
                break;
            case 3:
                promoteTo = 'B';
                break;
            case 4:
                promoteTo = 'R';
                break;
            default:
                return -1;
        }
        // Modify the current board
        this->board[this->promotionPos->x][this->promotionPos->y].at(1) = promoteTo;
        // Adjust the log
        this->inPromotion = false;
        return 0;
    }
    return -1;
}

void GameState::getValidMoves() {
    this->validMoves.clear();
    this->getAllMoves(this->validMoves); // get all moves
    this->addCastlingMoves(this->validMoves); // add castling if player is allowed to
    this->removeIllegalMoves(this->validMoves); // remove the invalid moves
}

void GameState::getAllMoves(std::list <std::array<Vector, 2>> &list) {
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

void GameState::removeIllegalMoves(std::list <std::array<Vector, 2>> &list) {
    // Loop through the list (we have to check i < list.size() since we use an unsigned int which will never be < 0, which means that else this would be an infinite loop)
    for (unsigned int i = list.size() - 1; i >= 0 && i < list.size(); i--) {
        // Get the array and store the piece that is in the destination square so we can undo it later
        std::array<Vector, 2> arr = *next(list.begin(), i);
        std::string occupiedSquare = this->board[arr[1].x][arr[1].y];
        std::map<char, Vector> tmp = this->kingPositions; // Store king positions
        this->move(arr[0].x, arr[0].y, arr[1].x, arr[1].y); // make the move
        if (this->isCheck()) {
            // The King is in check and therefore that piece is not allowed to be moved
            list.erase(next(list.begin(), i));// Remove the move from the list (remove by iterator)
        }
        this->undo(arr, occupiedSquare); // Undo move
        this->kingPositions = tmp; // Undo king positions
    }
}

void GameState::undo(std::array<Vector, 2> arr, std::string occupiedSquare) {
    this->board[arr[0].x][arr[0].y] = this->board[arr[1].x][arr[1].y];
    this->board[arr[1].x][arr[1].y] = std::move(occupiedSquare);
}

bool GameState::isCheck() {
    return this->squareUnderAttack(this->isWhiteToMove ? this->kingPositions.at('w') : this->kingPositions.at('b'));
}

bool GameState::squareUnderAttack(Vector pos) {
    this->isWhiteToMove = !this->isWhiteToMove; // change turn
    // get all the moves that the enemy player could make the next move
    std::list <std::array<Vector, 2>> enemyMoves;
    this->getAllMoves(enemyMoves);
    this->isWhiteToMove = !this->isWhiteToMove; // change the turn back
    // loop through moves and check if the endPos equals pos.x pos.y
    for (std::array<Vector, 2> arr : enemyMoves) {
        if (arr[1].x == pos.x && arr[1].y == pos.y) {
            return true;
        }
    }
    return false;
}

void GameState::addPawnMove(int r, int c, std::list <std::array<Vector, 2>> &list) {
    int offset = this->isWhiteToMove ? -1 : 1;
    // Normal pawn advances
    if (r + offset <= 7 && r + offset >= 0 &&
        this->board[r + offset][c] == "  ") { // Single pawn advance (Check field in front of the pawn)
        list.push_back(std::array < Vector, 2 > {*new Vector(r, c), *new Vector(r + offset, c)});
        if ((this->isWhiteToMove && r == 6 || !this->isWhiteToMove && r == 1) && this->board[r + (2 * offset)][c] ==
                                                                                 "  ") { // Double pawn advance (Check the field 2 in front of the pawn)
            list.push_back(std::array < Vector, 2 > {*new Vector(r, c), *new Vector(r + (2 * offset), c)});
        }
    }
    // Captures
    char enemy = this->isWhiteToMove ? 'b' : 'w';
    if (r + offset <= 7 && r + offset >= 0 && c + 1 <= 7 &&
        (this->board[r + offset][c + 1]).at(0) == enemy) { // Capture to the right
        list.push_back(std::array < Vector, 2 > {*new Vector(r, c), *new Vector(r + offset, c + 1)});
    }
    if (r + offset <= 7 && r + offset >= 0 && c - 1 >= 0 &&
        (this->board[r + offset][c - 1]).at(0) == enemy) { // Capture to the left
        list.push_back(std::array < Vector, 2 > {*new Vector(r, c), *new Vector(r + offset, c - 1)});
    }
    // En passant (Only possible if white is in row 3 or black is on row 4 (log.size should not be emtpy. The only
    // way that log is empty, is when creating custom positions, which should be possible)
    if (this->log.size() > 1 && (this->isWhiteToMove && r == 3 || !this->isWhiteToMove && r == 4)) {
        auto lastBoard = (*next(this->log.end(), -2)).board;  // Get the last board
        // Check if the last move was a 2 step pawn advance either left or right of the current column
        if (c - 1 >= 0 && lastBoard[r + (2 * offset)][c - 1].at(1) == 'P' &&
            this->board[r + offset][c - 1] == "  " && this->board[r + (2 * offset)][c - 1] == "  ") {
            list.push_back(std::array < Vector, 2 > {*new Vector(r, c), *new Vector(r + offset, c - 1)});
        } else if (c + 1 <= 7 && lastBoard[r + (2 * offset)][c + 1].at(1) == 'P' &&
                   this->board[r + offset][c + 1] == "  " && this->board[r + (2 * offset)][c + 1] == "  ") {
            list.push_back(std::array < Vector, 2 > {*new Vector(r, c), *new Vector(r + offset, c + 1)});
        }

    }
}

void GameState::addRookMove(int r, int c, std::list <std::array<Vector, 2>> &list) {
    int rows[4] = {-1, 1, 0, 0};
    int cols[4] = {0, 0, 1, -1};
    this->bishopRookHelper(rows, cols, r, c, list);
}

void GameState::addBishopMove(int r, int c, std::list <std::array<Vector, 2>> &list) {
    int rows[4] = {-1, 1, -1, 1};
    int cols[4] = {-1, 1, 1, -1};
    this->bishopRookHelper(rows, cols, r, c, list);
}

void GameState::bishopRookHelper(const int *rows, const int *cols, int r, int c,
                                 std::list <std::array<Vector, 2>> &list) {
    char turn = this->isWhiteToMove ? 'w' : 'b';
    char enemy = this->isWhiteToMove ? 'b' : 'w';
    for (int i = 0; i < 4; i++) {
        for (int row = r + rows[i], col = c + cols[i];
             row < 8 && row >= 0 && col < 8 && col >= 0; row += rows[i], col += cols[i]) {
            if ((this->board[row][col]).at(0) == turn) break; // Own Piece, so no need to keep on checking
            list.push_back(std::array < Vector, 2 > {*new Vector(r, c), *new Vector(row, col)});
            if ((this->board[row][col]).at(0) == enemy)
                break; // Enemy Piece, so allow to capture, but dont go further
        }
    }
}

void GameState::addKnightMove(int r, int c, std::list <std::array<Vector, 2>> &list) {
    char turn = this->isWhiteToMove ? 'w' : 'b';
    int rows[8] = {-2, -2, 2, 2, -1, 1, -1, 1};
    int cols[8] = {-1, 1, -1, 1, -2, -2, 2, 2};
    for (int i = 0; i < 8; i++) {
        int row = r + rows[i];
        int col = c + cols[i];
        // Inside the board and not attacking an ally
        if (row >= 0 && row < 8 && col >= 0 && col < 8 && (this->board[row][col]).at(0) != turn) {
            list.push_back(std::array < Vector, 2 > {*new Vector(r, c), *new Vector(row, col)});
        }
    }
}

void GameState::addQueenMove(int r, int c, std::list <std::array<Vector, 2>> &list) {
    this->addRookMove(r, c, list);
    this->addBishopMove(r, c, list);
}

void GameState::addKingMove(int r, int c, std::list <std::array<Vector, 2>> &list) {
    char turn = this->isWhiteToMove ? 'w' : 'b';
    int rows[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int cols[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    for (int i = 0; i < 8; i++) {
        int row = r + rows[i];
        int col = c + cols[i];
        if (row >= 0 && row < 8 && col >= 0 && col < 8 && (this->board[row][col]).at(0) != turn) {
            list.push_back(std::array < Vector, 2 > {*new Vector(r, c), *new Vector(row, col)});
        }
    }
}

void GameState::addCastlingMoves(std::list <std::array<Vector, 2>> &list) {
    char turn = this->isWhiteToMove ? 'w' : 'b';
    int r = this->kingPositions.at(turn).x;
    int c = this->kingPositions.at(turn).y;
    if (this->castleRights.at(turn)[0]) {
        // Check if the castling squares are not under attack and clear
        if (this->board[r][c - 1] == "  " && this->board[r][c - 2] == "  " && this->board[r][c - 3] == "  ") {
            if (!this->squareUnderAttack(*new Vector(r, c)) && !this->squareUnderAttack(*new Vector(r, c - 1)) &&
                !this->squareUnderAttack(*new Vector(r, c - 2))) {
                // Castle long
                list.push_back(std::array < Vector, 2 > {*new Vector(r, c), *new Vector(r, c - 2)});
            }
        }
    }
    if (this->castleRights.at(turn)[1]) {
        // Check if the castling squares are not under attack and clear
        if (this->board[r][c + 1] == "  " && this->board[r][c + 2] == "  ") {
            if (!this->squareUnderAttack(*new Vector(r, c)) && !this->squareUnderAttack(*new Vector(r, c + 1)) &&
                !this->squareUnderAttack(*new Vector(r, c + 2))) {
                // Castle short
                list.push_back(std::array < Vector, 2 > {*new Vector(r, c), *new Vector(r, c + 2)});
            }
        }
    }
}

