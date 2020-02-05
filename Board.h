#ifndef BOARD_H_
#define BOARD_H_

#include <string>
#include <vector>
#include <set>

struct Coord
{
  Coord(int x = 0, int y = 0);
  int x;
  int y;
};

struct Board
{
  Board(int s);
  void init();
  bool valid_coord(Coord coord) const;
  bool occupy(int player, Coord coord);
  inline int coord_trans(Coord coord) const;
  std::string icon(Coord coord) const;
  void print() const;
  int size{0};
  int step_count{0};
  std::vector<int> occupied;
};

#endif
