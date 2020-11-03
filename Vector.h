#ifndef CHESSENGINE_CPP_VECTOR_H
#define CHESSENGINE_CPP_VECTOR_H


class Vector {
public:
    int x, y;

    Vector(int x, int y);

    bool equals(Vector v) const;
};


#endif //CHESSENGINE_CPP_VECTOR_H
