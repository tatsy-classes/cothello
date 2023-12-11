#include "cothello.h"

#ifdef _OPENMP
#include <omp.h>
#endif

const int directions[8][2] = {
    { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 }, { 1, 1 }, { -1, -1 }, { 1, -1 }, { -1, 1 },
};

enum class Player : int {
    NONE = 0,
    BLACK = 1,
    WHITE = -1,
};

bool c_is_done(int *board) {
    bool is_done = true;
    int *ptr = board;
    for (int i = 0; i < 64; ++i, ++ptr) {
        if ((*ptr) == 0) {
            is_done = false;
            break;
        }
    }

    if (is_done) {
        return true;
    }

    std::vector<int> xs;
    std::vector<int> ys;

    c_legal_moves((int)Player::BLACK, board, xs, ys);
    if (!xs.empty() && !ys.empty()) {
        return false;
    }

    c_legal_moves((int)Player::WHITE, board, xs, ys);
    if (!xs.empty() && !ys.empty()) {
        return false;
    }

    return true;
}

bool c_is_legal_move(int player, int x, int y, int *board) {
    const int other = -1 * player;
    if (board[x * 8 + y] != (int)Player::NONE) {
        return false;
    }

    for (int i = 0; i < 8; i++) {
        const int dx = directions[i][0];
        const int dy = directions[i][1];
        int nx = x + dx;
        int ny = y + dy;
        if (nx < 0 || ny >= 8 || ny < 0 || nx >= 8) {
            continue;
        }

        if (board[nx * 8 + ny] != other) {
            continue;
        }

        while (1) {
            nx += dx;
            ny += dy;
            if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8) {
                break;
            }

            if (board[nx * 8 + ny] == (int)Player::NONE) {
                break;
            }

            if (board[nx * 8 + ny] == player) {
                return true;
            }
        }
    }

    return false;
}

void c_legal_moves(int player, int *board, std::vector<int> &xs, std::vector<int> &ys) {
    int count = 0;
    int xs_[64];
    int ys_[64];
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (c_is_legal_move(player, x, y, board)) {
                xs_[count] = x;
                ys_[count] = y;
                count++;
            }
        }
    }

    xs.assign(xs_, xs_ + count);
    ys.assign(ys_, ys_ + count);
}

void c_update(int player, int x, int y, int *board) {
    board[x * 8 + y] = player;

    const int other = -1 * player;
#pragma omp parallel for
    for (int i = 0; i < 8; i++) {
        const int dx = directions[i][0];
        const int dy = directions[i][1];
        int nx = x;
        int ny = y;
        bool inside = true;
        while (1) {
            nx += dx;
            ny += dy;
            if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8) {
                inside = false;
                break;
            }

            if (board[nx * 8 + ny] != other) {
                break;
            }
        }

        if (inside && board[nx * 8 + ny] == player) {
            while (nx != x || ny != y) {
                nx -= dx;
                ny -= dy;
                board[nx * 8 + ny] = player;
            }
        }
    }
}
