#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "AI_Player.h"

using std::shared_ptr;
using std::make_shared;
using std::vector;
using std::string;
using std::max;
int MAX = 1e4, MIN = -MAX;

AI_Player::AI_Player(unsigned depth) : _max_depth(depth),
                                       _player_shapes(3, vector<int>(2, 0)),
                                       _opponent_shapes(2, vector<int>(2, 0)) {}

Coord AI_Player::select_point(Board *board)
{
  _ab_nega_max(*board, MIN, MAX, 1);
  return _selection;
}

bool AI_Player::_COMPARE_ACTION(const Action &lhs, const Action &rhs)
{
  if(lhs.priority > rhs.priority)
    return true;
  if(lhs.priority == rhs.priority)
  {
    int r = rand() % 2;
    return r;
  }
  return false;
}

int AI_Player::_ab_nega_max(Board board, int alpha, int beta, int depth, Coord coord)
{
  if(depth > 1 && _terminal_test(&board, coord)) // in depth 1, terminal test is not necessary
    return -10000;

  if(depth > _max_depth)
    return -_heuristic(&board);

  Coord selec;
  int v = MIN;
  auto actions = _actions(&board);
  for(auto a : *actions)
  {
    int w = -_ab_nega_max(_result(&board, a.coord), -beta, -alpha, depth + 1, a.coord);
    if(w > v)
    {
      v = w;
      selec = a.coord;
    }
    if(v >= beta)
      break;
    alpha = max(alpha, v);
  }

  if(depth == 1)
    _selection = selec;

  return v;
}

bool AI_Player::_terminal_test(Board *board, Coord coord) const
{
  // vertical connection
  if(_terminal_test_dir(board, coord, {1, 0}))
    return true;

  // horizontal connection
  if(_terminal_test_dir(board, coord, {0, 1}))
    return true;

  // positive diagonal connection
  if(_terminal_test_dir(board, coord, {1, 1}))
    return true;
  
  // negative diagonal connection
  if(_terminal_test_dir(board, coord, {-1, 1}))
    return true;
  
  return false;
}

bool AI_Player::_terminal_test_dir(Board *board, Coord coord, Direction dir) const
{
  int player = (board->step_count - 1) % 2;

  Coord front_coord = coord - dir;
  bool connec = true;
  while(connec)
  {
    if(board->valid_coord(front_coord) &&
       board->occupied[board->coord_trans(front_coord)] == player)
      front_coord = front_coord - dir;
    else
      connec = false;
  }

  Coord back_coord = coord + dir;
  connec = true;
  while(connec)
  {
    if(board->valid_coord(back_coord) &&
       board->occupied[board->coord_trans(back_coord)] == player)
      back_coord = back_coord + dir;
    else
      connec = false;
  }

  return back_coord.y - front_coord.y - 1 >= 5 ||
         back_coord.x - front_coord.x - 1 >= 5;
}

inline int AI_Player::_player(Board *board) const
{
  return board->step_count % 2;
}

shared_ptr<vector<Action>> AI_Player::_actions(Board *board) const
{
  int size = board->size;
  auto actions = make_shared<vector<Action>>();
  if(!board->step_count)
  {
    Action act;
    act.coord = {size / 2, size / 2};
    actions->push_back(act);
    return actions;
  }

  for(int x = 1; x <= size; ++x)
    for(int y = 1; y <= size; ++y)
      if(board->occupied[board->coord_trans({x, y})] == -1)
      {
        int neigh_amount = _neighbore(board, {x, y});
        if(neigh_amount)
        {
          Action act;
          act.priority = neigh_amount;
          act.coord = {x, y};
          actions->push_back(act);
        }
      }

  srand(time(nullptr));
  std::sort(actions->begin(), actions->end(), _COMPARE_ACTION);
  return actions;
}

int AI_Player::_neighbore(Board *board, Coord coord) const
{
  int neigh_amount = 0;
  Coord neigh = coord;
  --neigh.x;
  --neigh.y;
  _check_neigh(board, neigh, &neigh_amount);
  ++neigh.x;
  _check_neigh(board, neigh, &neigh_amount);
  ++neigh.x;
  _check_neigh(board, neigh, &neigh_amount);
  ++neigh.y;
  _check_neigh(board, neigh, &neigh_amount);
  ++neigh.y;
  _check_neigh(board, neigh, &neigh_amount);
  --neigh.x;
  _check_neigh(board, neigh, &neigh_amount);
  --neigh.x;
  _check_neigh(board, neigh, &neigh_amount);
  --neigh.y;
  _check_neigh(board, neigh, &neigh_amount);

  return neigh_amount;
}

inline void AI_Player::_check_neigh(Board *board, Coord coord, int *amount) const
{
  if(board->valid_coord(coord) && board->occupied[board->coord_trans(coord)] != -1)
    ++*amount;
}

Board AI_Player::_result(Board *board, Coord coord) const
{
  Board result = *board;
  result.occupy(_player(board), coord);
  return result;
}

int AI_Player::_heuristic(Board *board)
{
  // init data of chess shape
  for(int i = 0; i < 3; ++i)
    for(int j = 0; j < 2; ++j)
      _player_shapes[i][j] = 0;

  for(int i = 0; i < 2; ++i)
    for(int j = 0; j < 2; ++j)
      _opponent_shapes[i][j] = 0;

  int size = board->size;

  int blank_preifix;
  bool player_prefix, opponent_prefix;

  // analysis chess shape in each horizontal lines
  for(int y = 1; y <= size; ++y)
  {
    blank_preifix = 0;
    player_prefix = opponent_prefix = true;
    for(int x = 1; x <= size - 1; ++x)
    {
      int h = _analysis(board, {x, y}, {1, 0}, &blank_preifix, &player_prefix, &opponent_prefix);
      if(h)
        return h;
    }
  }

  // analysis chess shape in each vertical lines
  for(int x = 1; x <= size; ++x)
  {
    blank_preifix = 0;
    player_prefix = opponent_prefix = true;
    for(int y = 1; y <= size - 1; ++y)
    {
      int h = _analysis(board, {x, y}, {0, 1}, &blank_preifix, &player_prefix, &opponent_prefix);
      if(h)
        return h;
    }
  }

  // analysis chess shape in each positive diagonal lines
  for(int i = 1; i <= size * 2 - 1; ++i)
  {
    blank_preifix = 0;
    player_prefix = opponent_prefix = true;
    for(int j = 1; j <= (i <= size ? size - i : 2 * size - i - 1); ++j)
    {
      int x, y;
      if(i <= size)
      {
        x = i + j - 1;
        y = j;
      }
      else
      {
        x = j;
        y = i - size + j;
      }
      int h = _analysis(board, {x, y}, {1, 1}, &blank_preifix, &player_prefix, &opponent_prefix);
      if(h)
        return h;
    }
  }

  // analysis chess shape in each negative diagonal lines
  for(int i = 1; i <= size * 2 - 1; ++i)
  {
    blank_preifix = 0;
    player_prefix = opponent_prefix = true;
    for(int j = 1; j <= (i <= size ? size - i : 2 * size - i - 1); ++j)
    {
      int x, y;
      if(i <= size)
      {
        x = size - i - j + 2;
        y = j;
      }
      else
      {
        x = size - j + 1;
        y = i - size + j;
      }
      int h = _analysis(board, {x, y}, {-1, 1}, &blank_preifix, &player_prefix, &opponent_prefix);
      if(h)
        return h;
    }
  }

  // staic all chess shapes on the board
  // player has 活四 or has 沖四 more then one
  if(_player_shapes[2][1] || _player_shapes[2][0] > 1)
    return 9030;
  // player has 沖四 and 活三
  if(_player_shapes[2][0] && _player_shapes[1][1])
    return 9020;
  // player do not have 沖四, but opponent has 活三
  if(!_player_shapes[2][0] && _opponent_shapes[1][1])
    return -9010;
  // player has 活三 more then one, and opponent do not have 活三 or 眠三
  if(_player_shapes[1][1] > 1 && !_opponent_shapes[1][0] && !_opponent_shapes[1][1])
    return 9000;
  
  int h = 0;
  if(_player_shapes[2][0])
    h += 2000;
  
  if(_player_shapes[1][1] > 1)
    h += 500;
  else if(_player_shapes[1][1])
    h += 100;
  
  if(_opponent_shapes[1][1] > 1)
    h -= 2000;
  else if(_opponent_shapes[1][1])
    h -= 500;
  
  h += (_player_shapes[1][0] - _opponent_shapes[1][0]) * 10;
  h += (_player_shapes[0][1] - _opponent_shapes[0][1]) * 4;
  h += (_player_shapes[0][0] - _opponent_shapes[0][0]) * 2;

  return h;
}

int AI_Player::_analysis(Board *board, Coord coord, Direction dir,
                         int *blank_prefix, bool *player_prefix, bool *opponent_prefix)
{
  int player = (board->step_count - 1) % 2;

  int owner = board->occupied[board->coord_trans({coord})];
  if(owner == -1) // coodinate is a blank
  {
    ++*blank_prefix;
    *player_prefix = *opponent_prefix = true;
  }
  else if(owner == player) // coordinate is occupied by the player
  {
    if(*opponent_prefix) // in order to avoid analysising repeatedly
    {
      Chess_Shape shape = _analysis_line(board, coord, dir, *blank_prefix);
      if(shape.amount > 1)
        ++_player_shapes[shape.amount - 2][shape.alive];
    }
    *blank_prefix = 0;
    *player_prefix = true;
    *opponent_prefix = false;
  }
  else // coordinate was occupied by the opponent
  {
    if(*player_prefix)
    {
      Chess_Shape shape = _analysis_line(board, coord, dir, *blank_prefix);

      if(shape.amount == 4) // if opponent has 活四 or 沖四, the player must loose
        return shape.alive ? -9050 : -9040;
      
      if(shape.amount > 1)
        ++_opponent_shapes[shape.amount - 2][shape.alive];
    }
    *blank_prefix = 0;
    *player_prefix = false;
    *opponent_prefix = true;
  }

  return 0; // return 0 means the heuristic is not already be determined
}

Chess_Shape AI_Player::_analysis_line(Board *board, Coord coord, Direction dir, int blank_prefix) const
{
  int player = board->occupied[board->coord_trans(coord)];
  int opponent = player == 1 ? 0 : 1;

  int shape_int = 1;
  int i = 2;
  bool blocked = false;
  coord = coord + dir;
  while(i++ <= 5 && !blocked)
  {
    if(!board->valid_coord(coord) ||
       board->occupied[board->coord_trans(coord)] == opponent)
      blocked = true;
    else
    {
      shape_int = shape_int << 1;
      shape_int += board->occupied[board->coord_trans(coord)] == player ? 1 : 0;
    }

    coord = coord + dir;
  }

  Chess_Shape shape;
  shape.amount = 1;
  switch(shape_int)
  {
    case 0b10001:
      shape.amount = 2;
      shape.alive = false;
      break;
    
    case 0b11:
      if(blank_prefix >= 3)
      {
        shape.amount = 2;
        shape.alive = false;
      }
      break;
    
    case 0b110:
      if(blank_prefix >= 2)
      {
        shape.amount = 2;
        shape.alive = true;
      }
      break;
    
    case 0b1100:
      if(blank_prefix > 0)
      {
        shape.amount = 2;
        shape.alive = blank_prefix >= 2 ? true : false;
      }
      break;
    
    case 0b11000:
      shape.amount = 2;
      shape.alive = blank_prefix > 0 ? true : false;
      break;
    
    case 0b101:
      if(blank_prefix >= 2)
      {
        shape.amount = 2;
        shape.alive = false;
      }
      break;
    
    case 0b1010:
      if(blank_prefix > 0)
      {
        shape.amount = 2;
        shape.alive = true;
      }
      break;
    
    case 0b10100:
      shape.amount = 2;
      shape.alive = blank_prefix > 0 ? true : false;
      break;
    
    case 0b1001:
      if(blank_prefix > 0)
      {
        shape.amount = 2;
        shape.alive = false;
      }
      break;
    
    case 0b10010:
      shape.amount = 2;
      shape.alive = blank_prefix > 0 ? true : false;
      break;
    
    case 0b111:
      if(blank_prefix >= 2)
      {
        shape.amount = 3;
        shape.alive = false;
      }
      break;
    
    case 0b1110:
      if(blank_prefix > 0)
      {
        shape.amount = 3;
        shape.alive = blank_prefix >= 2 ? true : false;
      }
      break;
    
    case 0b11100:
      shape.amount = 3;
      shape.alive = blank_prefix > 0 ? true : false;
      break;
    
    case 0b1101: case 0b1011:
      if(blank_prefix > 0)
      {
        shape.amount = 3;
        shape.alive = false;
      }
      break;
    
    case 0b11010: case 0b10110:
      shape.amount = 3;
      shape.alive = blank_prefix > 0 ? true : false;
      break;
    
    case 0b11001: case 0b10101:
      shape.amount = 3;
      shape.alive = false;
      break;
    
    case 0b1111:
      if(blank_prefix > 0)
      {
        shape.amount = 4;
        shape.alive = false;
      }
      break;
    
    case 0b11110:
      shape.amount = 4;
      shape.alive = blank_prefix > 0 ? true : false;
      break;
    
    case 0b11101: case 0b11011:
      shape.amount = 4;
      shape.alive = false;
      break;
    
    default:
      break;
  }

  return shape;
}
