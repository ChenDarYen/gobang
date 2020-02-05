#ifndef DIRECTION_H_
#define DIRECTION_H_

#include "Board.h"

struct Direction
{
  Direction(int x, int y);
  int x;
  int y;
};

Coord operator+(const Coord &coord, const Direction &dir);

Coord operator-(const Coord &coord, const Direction &dir);

#endif
