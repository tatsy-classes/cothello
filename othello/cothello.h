#ifndef COTHELLO_H
#define COTHELLO_H

#include <vector>

bool c_is_done(int *board);
bool c_is_legal_move(int player, int x, int y, int *board);
void c_legal_moves(int player, int *board, std::vector<int> &xs, std::vector<int> &ys);
void c_update(int player, int x, int y, int *board);

#endif  // COTHELLO_H
