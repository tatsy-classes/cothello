cimport cython
from libcpp cimport bool
from libcpp.vector cimport vector


cdef extern from "cothello.h":
    bool c_is_done(int *board)
    bool c_is_legal_move(int player, int x, int y, int *board)
    void c_legal_moves(int player, int *board, vector[int] &xs, vector[int] &ys)
    void c_update(int player, int x, int y, int *board)


@cython.boundscheck(False)
@cython.wraparound(False)
cpdef bool cython_is_done(int[:, :] board):
    return c_is_done(&board[0, 0])


@cython.boundscheck(False)
@cython.wraparound(False)
cpdef tuple cython_legal_moves(const int player, int[:, :] board):
    cdef vector[int] xs = []
    cdef vector[int] ys = []
    c_legal_moves(player, &board[0, 0], xs, ys)
    return xs, ys


@cython.boundscheck(False)
@cython.wraparound(False)
cpdef void cython_update(const int player, const int x, const int y, int[:, :] board):
    c_update(player, x, y, &board[0, 0])
