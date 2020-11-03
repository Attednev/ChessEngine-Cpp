#ifndef CHESSENGINE_CPP_LOGENTRY_H
#define CHESSENGINE_CPP_LOGENTRY_H

#include <iostream>
#include <map>
#include <string>
#include "Vector.h"

class LogEntry {
public:
    std::string board[8][8];
    bool isWhiteToMove;
    std::map<char, Vector> kingPositions;
    std::map<char, std::array<bool, 2>> castleRights;

    LogEntry(std::string board[8][8], bool isWhiteToMove, std::map<char, Vector> kingPositions,
             std::map<char, std::array<bool, 2>> castleRights);
};


#endif //CHESSENGINE_CPP_LOGENTRY_H
