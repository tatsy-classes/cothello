import numpy as np
import othello.libcpp.bitboard as bb


def test_coordinates_to_bits():
    for j, x in enumerate("ABCDEFGH"):
        for i, y in enumerate("12345678"):
            xy = x + y
            b = bb.c_coordinates_to_bits(xy)
            board = bb.c_bits_to_board(b)
            board = np.array(board).reshape((8, 8))
            assert board[i, j] == 1


def test_make_legal_board():
    b0 = np.uint64(0x0000_0008_1000_0000)
    b1 = np.uint64(0x0000_0010_0800_0000)
    leg = bb.c_make_legal_board(b0, b1)
    print("")
    print(bb.c_bits_to_board(leg))


def test_can_put():
    b0 = np.uint64(0x0000_0008_1000_0000)
    b1 = np.uint64(0x0000_0010_0800_0000)

    put = bb.c_coordinates_to_bits("D3")
    assert bb.c_can_put(put, b0, b1)

    put = bb.c_coordinates_to_bits("C4")
    assert bb.c_can_put(put, b0, b1)

    put = bb.c_coordinates_to_bits("B2")
    assert not bb.c_can_put(put, b0, b1)


def test_reverse():
    b0 = np.uint64(0x0000_0008_1000_0000)
    b1 = np.uint64(0x0000_0010_0800_0000)

    put = bb.c_coordinates_to_bits("D3")
    assert bb.c_can_put(put, b0, b1)

    b0, b1 = bb.c_reverse(put, b0, b1)
    print("")
    print(bb.c_bits_to_board(b0))
    print(bb.c_bits_to_board(b1))
