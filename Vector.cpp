#include "Vector.h"

Vector::Vector(int x, int y) {
    this->x = x;
    this->y = y;
}

bool Vector::equals(Vector v) const {
    return (this->x == v.x && this->y == v.y);
}