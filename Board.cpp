#include <cmath>
#include <iostream>
#include "Board.h"

using std::cout;
using std::endl;

Coord::Coord(int x, int y) : x(x), y(y) {}

Board::Board(int s) : size(s), occupied(s * s, -1) {}

void Board::init()
{
  step_count = 0;
  for(auto it = occupied.begin(), end = occupied.end(); it != end; ++it)
    *it = -1;
}

bool Board::valid_coord(Coord coord) const
{
  if(coord.x < 1 || coord.x > size || coord.y < 1 || coord.y > size)
    return false;

  return true;
}

bool Board::occupy(int player, Coord coord)
{
  if(!valid_coord(coord) || occupied[coord_trans(coord)] != -1)
    return false;
  if(player != 0 && player != 1)
    return false;

  occupied[coord_trans(coord)] = player;
  ++step_count;

  return true;
}

inline int Board::coord_trans(Coord coord) const
{
  return (coord.y - 1) * size + coord.x - 1;
}

std::string Board::icon(Coord coord) const
{
  switch(occupied[coord_trans(coord)])
  {
    case 0:
      return std::string("O");
      break;
    
    case 1:
      return std::string("X");
      break;
    
    default:
      return std::string(" ");
  }
}

void Board::print() const
{
  cout << "    ";
  for(int i = 1; i <= size; ++i)
    cout << i << (i < 10 ? "  " : " ");
  cout << "\n";

  for(int i = 1; i <= size; ++i)
  {
    cout << "   ";
    for(int j = 1; j <= size; ++j)
      cout << "|--";
    cout << "|\n";

    cout << (i < 10 ? " " : "") << i << " ";
    for(int j = 1; j <= size; ++j)
      cout << "|" << icon(Coord(j, i)) << " ";
    cout << "|\n";
  }

  cout << "   ";
  for (int j = 1; j <= size; ++j)
    cout << "|--";
  cout << "|" << endl;
}
