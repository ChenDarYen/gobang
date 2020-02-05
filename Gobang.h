#ifndef GOBANG_H_
#define GOBANG_H_

#include "Board.h"
#include "Direction.h"
#include "AI_Player.h"

class Gobang
{
public:
  Gobang(Board &board, AI_Player &AI);
  void play();

private:
  void _init();
  void _print_board() const;
  bool _place(int player); // return if the player win after this turn
  bool _victory(Coord coord) const;
  bool _victory_dir(Coord coord, Direction dir) const; // dir can only be {1, 0}, {0 ,1}, {1, 1}, {-1, 1}
  Board *_board;
  AI_Player *_AI;
  int _computer{0};
  bool _end{false};
};

#endif
