#include "LogEntry.h"

LogEntry::LogEntry(std::string board[8][8], bool isWhiteToMove, std::map<char, Vector> kingPositions,
                   std::map<char, std::array<bool, 2>> castleRights) {
    copy(&board[0][0], &board[0][0] + 64, &this->board[0][0]);
    this->isWhiteToMove = isWhiteToMove;
    this->kingPositions = std::move(kingPositions);
    this->castleRights = std::move(castleRights);
}
