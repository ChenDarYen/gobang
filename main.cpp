#include <iostream>

#include <exception>
#include "Direction.cpp"
#include "Board.cpp"
#include "Gobang.cpp"
#include "AI_Player.cpp"

int main()
{
  Board board(10);
  AI_Player AI(7);
  Gobang gobang(board, AI);
  gobang.play();

  return 0;
}