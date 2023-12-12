import numpy as np
from othello import bitboard as bb


def test_xy2bits():
    for j, x in enumerate("ABCDEFGH"):
        for i, y in enumerate("12345678"):
            xy = x + y
            b = bb.cython_coordinatesToBits(xy)
            board = bb.cython_bitsToBoard(b)
            assert board[i, j] == 1


def test_legal():
    b0 = np.uint64(0x0000_0008_1000_0000)
    b1 = np.uint64(0x0000_0010_0800_0000)
    leg = bb.cython_makeLegalBoard(b0, b1)
    print("")
    print(bb.cython_bitsToBoard(leg))


def test_canPut():
    b0 = np.uint64(0x0000_0008_1000_0000)
    b1 = np.uint64(0x0000_0010_0800_0000)

    put = bb.cython_coordinatesToBits("D3")
    assert bb.cython_canPut(put, b0, b1)

    put = bb.cython_coordinatesToBits("C4")
    assert bb.cython_canPut(put, b0, b1)

    put = bb.cython_coordinatesToBits("B2")
    assert not bb.cython_canPut(put, b0, b1)


def test_reverse():
    b0 = np.uint64(0x0000_0008_1000_0000)
    b1 = np.uint64(0x0000_0010_0800_0000)

    put = bb.cython_coordinatesToBits("D3")
    assert bb.cython_canPut(put, b0, b1)

    b0, b1 = bb.cython_reverse(put, b0, b1)
    print("")
    print(bb.cython_bitsToBoard(b0))
    print(bb.cython_bitsToBoard(b1))
