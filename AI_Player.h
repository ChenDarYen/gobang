#ifndef AI_PLAYER_H_
#define AI_PLAYER_H_

#include <vector>
#include "Board.h"
#include "Direction.h"

struct Action
{
  int priority{0};
  Coord coord{0, 0};
};

struct Chess_Shape
{
  int amount{0};
  bool alive{true};
};

class AI_Player
{
public:
  AI_Player(unsigned depth);
  Coord select_point(Board *board);

private:
  int _ab_nega_max(Board *board, int alpha, int beta, int depth, Coord coord = {0, 0});
  bool _terminal_test(Board *board, Coord coord) const;
  bool _terminal_test_dir(Board *board, Coord coord, Direction dir) const; // dir can only be {1, 0}, {0 ,1}, {1, 1}, {-1, 1}
  inline int _player(Board *board) const;
  std::vector<Action> _actions(Board *board) const;
  int _neighbore(Board *board, Coord coord) const;
  inline void _check_neigh(Board *board, Coord coord, int *amount) const;
  int _heuristic(Board *board);
  int _analysis(Board *board, Coord coord, Direction dir,
                int *blank_prefix, bool *player_prefix, bool *opponent_prefix);
  Chess_Shape _analysis_line(Board *board, Coord coord, Direction dir, int blank_prefix) const; // dir can only be {1, 0}, {0 ,1}, {1, 1}, {-1, 1}
  unsigned _max_depth{0};
  Coord _selection{0, 0};
  std::vector<std::vector<int>> _player_shapes;
  std::vector<std::vector<int>> _opponent_shapes;
};

#endif
