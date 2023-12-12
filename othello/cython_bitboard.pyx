# cython: c_string_type=unicode, c_string_encoding=utf8

cimport cython
import numpy as np
cimport numpy as np
from libcpp cimport bool
from libcpp.string cimport string
from libc.stdint cimport uint64_t
from libcpp.vector cimport vector
from libcpp.pair cimport pair

cdef extern from "bitboard.h":
    uint64_t coordinatesToBits(string xy)
    vector[int] bitsToBoard(uint64_t bits)
    uint64_t makeLegalBoard(uint64_t b0, uint64_t b1)
    pair[uint64_t, uint64_t] reverse(uint64_t put, uint64_t b0, uint64_t b1)
    int bitCount(uint64_t bits)


@cython.boundscheck(False)
@cython.wraparound(False)
cpdef np.uint64_t cython_coordinatesToBits(string xy):
    return coordinatesToBits(xy)

@cython.boundscheck(False)
@cython.wraparound(False)
cpdef np.ndarray[np.int32_t, ndim=2] cython_bitsToBoard(np.uint64_t bits):
    b = bitsToBoard(bits)
    return np.array(b, dtype="int32").reshape((8, 8))

@cython.boundscheck(False)
@cython.wraparound(False)
cpdef np.uint64_t cython_makeLegalBoard(np.uint64_t b0, np.uint64_t b1):
    return makeLegalBoard(b0, b1)


@cython.boundscheck(False)
@cython.wraparound(False)
cpdef bool cython_canPut(np.uint64_t put, np.uint64_t b0, np.uint64_t b1):
    cdef uint64_t legal = makeLegalBoard(b0, b1)
    return (put & legal) == put

@cython.boundscheck(False)
@cython.wraparound(False)
cpdef tuple cython_reverse(np.uint64_t put, np.uint64_t b0, np.uint64_t b1):
    return reverse(put, b0, b1)

@cython.boundscheck(False)
@cython.wraparound(False)
cpdef bool cython_isPass(np.uint64_t b0, np.uint64_t b1):
    l0 = makeLegalBoard(b0, b1)
    l1 = makeLegalBoard(b1, b0)
    return (l0 == 0) and (l1 != 0)


@cython.boundscheck(False)
@cython.wraparound(False)
cpdef bool cython_isDone(np.uint64_t b0, np.uint64_t b1):
    l0 = makeLegalBoard(b0, b1)
    l1 = makeLegalBoard(b1, b0)
    return (l0 == 0) and (l1 == 0)

@cython.boundscheck(False)
@cython.wraparound(False)
cpdef int cython_bitCount(np.uint64_t bits):
    return bitCount(bits)
