#include <iostream>

#include <exception>
#include "Direction.cpp"
#include "Board.cpp"
#include "Gobang.cpp"
#include "AI_Player.cpp"

int main()
{
  Board board(10);
  AI_Player AI(4);
  Gobang gobang(board, AI);
  try
  {
    gobang.play();
  }
  catch(std::exception &e)
  {
    std::cout << e.what() << std::endl;
  }

  return 0;
}