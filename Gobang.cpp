#include <iostream>
#include <cstdlib>
#include "Gobang.h"

using std::cout;
using std::cin;
using std::endl;

Gobang::Gobang(Board &board, AI_Player &AI) : _board(&board), _AI(&AI) {}

void Gobang::play()
{
  while(true)
  {
    _init();

    cout << "you want to place first or second? (f/s)\n";
    char mode = 'f';
    cin >> mode;
    if(mode == 'f')
      _computer = 1;
    else
      _computer = 0;

    _print_board();

    int step = 0;
    int winner = -1;
    int bound = _board->size * _board->size;
    while(step < bound)
    {
      if(_place(step % 2))
      {
        winner = step % 2;
        break;
      }
      ++step;
    }

    if(winner == -1)
      cout << "tie.\n";
    else
      cout << (winner == _computer ? "computer" : "you") << " win.\n";

    cout << "play again? (y/n)";
    char ans;
    cin >> ans;
    if(ans == 'n')
      _end = true;

    if(_end)
      break;
    else
      system("clear");
  }
  exit(1);
}

void Gobang::_init()
{
  _board->init();
  _end = false;
}

void Gobang::_print_board() const
{
  system("clear");
  _board->print();
}

bool Gobang::_place(int player)
{
  Coord coord;

  if(player != _computer)
  {
    while(true)
    {
      cout << "please input chess coordinate:\n" << "x: ";
      cin >> coord.x;
      cout << "y: ";
      cin >> coord.y;
      if(_board->valid_coord(coord) &&
         _board->occupied[_board->coord_trans(coord)] == -1)
        break;
      else
        cout << "wrong position!\n";
    }
  }
  else
    coord = _AI->select_point(_board);
  
  cout << "place at: " << coord.x << ", " << coord.y << endl;

  _board->occupy(player, coord);
  _print_board();
  return _victory(coord);
}

bool Gobang::_victory(Coord coord) const
{
  // vertical connection
  if(_victory_dir(coord, {1, 0}))
    return true;

  // horizontal connection
  if(_victory_dir(coord, {0, 1}))
    return true;

  // positive diagonal connection
  if(_victory_dir(coord, {1, 1}))
    return true;
  
  // negative diagonal connection
  if(_victory_dir(coord, {-1, 1}))
    return true;
  
  return false;
}

bool Gobang::_victory_dir(Coord coord, Direction dir) const
{
  int player = (_board->step_count - 1) % 2;

  Coord front_coord = coord - dir;
  bool connec = true;
  while(connec)
  {
    if(_board->valid_coord(front_coord) &&
       _board->occupied[_board->coord_trans(front_coord)] == player)
      front_coord = front_coord - dir;
    else
      connec = false;
  }

  Coord back_coord = coord + dir;
  connec = true;
  while(connec)
  {
    if(_board->valid_coord(back_coord) &&
       _board->occupied[_board->coord_trans(back_coord)] == player)
      back_coord = back_coord + dir;
    else
      connec = false;
  }

  return back_coord.y - front_coord.y - 1 >= 5 ||
         back_coord.x - front_coord.x - 1 >= 5;
}
