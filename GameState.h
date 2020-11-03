#ifndef CHESSENGINE_CPP_GAMESTATE_H
#define CHESSENGINE_CPP_GAMESTATE_H

#include <iostream>
#include <list>
#include <array>
#include <map>
#include "Vector.h"
#include "LogEntry.h"

class GameState {
public:
    GameState();

    std::list <Vector> getMovesForPiece(int r, int c);

    void undoMove();

    int promote(int piece);

    int playMove(int startRow, int startCol, int endRow, int endCol);

    std::string board[8][8] = {
            {"bR", "bN", "bB", "bQ", "bK", "bB", "bN", "bR"},
            {"bP", "bP", "bP", "bP", "bP", "bP", "bP", "bP"},
            {"  ", "  ", "  ", "  ", "  ", "  ", "  ", "  "},
            {"  ", "  ", "  ", "  ", "  ", "  ", "  ", "  "},
            {"  ", "  ", "  ", "  ", "  ", "  ", "  ", "  "},
            {"  ", "  ", "  ", "  ", "  ", "  ", "  ", "  "},
            {"wP", "wP", "wP", "wP", "wP", "wP", "wP", "wP"},
            {"wR", "wN", "wB", "wQ", "wK", "wB", "wN", "wR"}
    };
private:
    void sideEffects(int startRow, int startCol, int endRow, int endCol);

    bool checkInsufficientMaterial();

    static bool checkIfListIsInStaleMate(std::list<char> pieces);

    void move(int startRow, int startCol, int endRow, int endCol);

    void getValidMoves();

    void getAllMoves(std::list <std::array<Vector, 2>> &list);

    void removeIllegalMoves(std::list <std::array<Vector, 2>> &list);

    void undo(std::array<Vector, 2> arr, std::string occupiedSquare);

    bool isCheck();

    bool squareUnderAttack(Vector pos);

    void addPawnMove(int r, int c, std::list <std::array<Vector, 2>> &list);

    void addRookMove(int r, int c, std::list <std::array<Vector, 2>> &list);

    void addBishopMove(int r, int c, std::list <std::array<Vector, 2>> &list);

    void bishopRookHelper(const int rows[], const int cols[], int r, int c, std::list <std::array<Vector, 2>> &list);

    void addKnightMove(int r, int c, std::list <std::array<Vector, 2>> &list);

    void addQueenMove(int r, int c, std::list <std::array<Vector, 2>> &list);

    void addKingMove(int r, int c, std::list <std::array<Vector, 2>> &list);

    void addCastlingMoves(std::list <std::array<Vector, 2>> &list);

    bool isWhiteToMove = true;
    bool inPromotion = false;
    Vector *promotionPos = new Vector(0, 0);
    std::list <std::array<Vector, 2>> validMoves;

    std::map<char, Vector> kingPositions = {
            {'w', Vector(7, 4)},
            {'b', Vector(0, 4)}
    };
    std::map<char, std::array<bool, 2>> castleRights = {
            {'w', std::array < bool, 2 > {true, true}},
            {'b', std::array < bool, 2 > {true, true}}
    };
    std::list <LogEntry> log;
};

#endif //CHESSENGINE_CPP_GAMESTATE_H
