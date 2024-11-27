#include <string>
#include <cstdint>
#include <vector>
#include <array>

#include "bitboard.h"

namespace py = pybind11;

uint64_t coordinatesToBits(const std::string &xy) {
    const char x = xy[0];
    const char y = xy[1];
    uint64_t mask = 0x8000'0000'0000'0000;

    // X shift
    const int shiftX = x - 'A';
    mask >>= shiftX;

    // Y shift
    const int shiftY = y - '1';
    mask >>= (shiftY * 8);

    return mask;
}

std::array<int, NUM_CELLS> bitsToBoard(uint64_t bits) {
    std::array<int, NUM_CELLS> board;
    std::fill(board.begin(), board.end(), 0);

    int pos = NUM_CELLS - 1;
    while (bits != 0) {
        board[pos--] = bits & 0x01;
        bits >>= 1;
    }
    return board;
}

uint64_t makeLegalBoard(uint64_t b0, uint64_t b1) {
    const uint64_t hWatchBoard = b1 & 0x7e7e'7e7e'7e7e'7e7e;
    const uint64_t vWatchBoard = b1 & 0x00ff'ffff'ffff'ff00;
    const uint64_t dWatchBoard = b1 & 0x007e'7e7e'7e7e'7e00;
    const uint64_t blankBoard = ~(b0 | b1);

    uint64_t tmp;
    uint64_t legal = 0;

    // Left
    tmp = hWatchBoard & (b0 << 1);
    tmp |= hWatchBoard & (tmp << 1);
    tmp |= hWatchBoard & (tmp << 1);
    tmp |= hWatchBoard & (tmp << 1);
    tmp |= hWatchBoard & (tmp << 1);
    tmp |= hWatchBoard & (tmp << 1);
    legal |= blankBoard & (tmp << 1);

    // Right
    tmp = hWatchBoard & (b0 >> 1);
    tmp |= hWatchBoard & (tmp >> 1);
    tmp |= hWatchBoard & (tmp >> 1);
    tmp |= hWatchBoard & (tmp >> 1);
    tmp |= hWatchBoard & (tmp >> 1);
    tmp |= hWatchBoard & (tmp >> 1);
    legal |= blankBoard & (tmp >> 1);

    // Top
    tmp = vWatchBoard & (b0 << 8);
    tmp |= vWatchBoard & (tmp << 8);
    tmp |= vWatchBoard & (tmp << 8);
    tmp |= vWatchBoard & (tmp << 8);
    tmp |= vWatchBoard & (tmp << 8);
    tmp |= vWatchBoard & (tmp << 8);
    legal |= blankBoard & (tmp << 8);

    // Bottom
    tmp = vWatchBoard & (b0 >> 8);
    tmp |= vWatchBoard & (tmp >> 8);
    tmp |= vWatchBoard & (tmp >> 8);
    tmp |= vWatchBoard & (tmp >> 8);
    tmp |= vWatchBoard & (tmp >> 8);
    tmp |= vWatchBoard & (tmp >> 8);
    legal |= blankBoard & (tmp >> 8);

    // Top-right
    tmp = dWatchBoard & (b0 << 7);
    tmp |= dWatchBoard & (tmp << 7);
    tmp |= dWatchBoard & (tmp << 7);
    tmp |= dWatchBoard & (tmp << 7);
    tmp |= dWatchBoard & (tmp << 7);
    tmp |= dWatchBoard & (tmp << 7);
    legal |= blankBoard & (tmp << 7);

    // Top-left
    tmp = dWatchBoard & (b0 << 9);
    tmp |= dWatchBoard & (tmp << 9);
    tmp |= dWatchBoard & (tmp << 9);
    tmp |= dWatchBoard & (tmp << 9);
    tmp |= dWatchBoard & (tmp << 9);
    tmp |= dWatchBoard & (tmp << 9);
    legal |= blankBoard & (tmp << 9);

    // Bottom-right
    tmp = dWatchBoard & (b0 >> 7);
    tmp |= dWatchBoard & (tmp >> 7);
    tmp |= dWatchBoard & (tmp >> 7);
    tmp |= dWatchBoard & (tmp >> 7);
    tmp |= dWatchBoard & (tmp >> 7);
    tmp |= dWatchBoard & (tmp >> 7);
    legal |= blankBoard & (tmp >> 7);

    // Bottom-left
    tmp = dWatchBoard & (b0 >> 9);
    tmp |= dWatchBoard & (tmp >> 9);
    tmp |= dWatchBoard & (tmp >> 9);
    tmp |= dWatchBoard & (tmp >> 9);
    tmp |= dWatchBoard & (tmp >> 9);
    tmp |= dWatchBoard & (tmp >> 9);
    legal |= blankBoard & (tmp >> 9);

    return legal;
}

std::tuple<uint64_t, uint64_t> reverse(uint64_t put, uint64_t b0, uint64_t b1) {
    uint64_t rev = 0;
    for (int k = 0; k < 8; k++) {
        uint64_t tmp = 0;
        uint64_t mask = transfer(put, k);
        while (mask != 0 && ((mask & b1) != 0)) {
            tmp |= mask;
            mask = transfer(mask, k);
        }

        if ((mask & b0) != 0) {
            rev |= tmp;
        }
    }

    b0 ^= put | rev;
    b1 ^= rev;

    return std::make_tuple(b0, b1);
}

uint64_t transfer(uint64_t put, int k) {
    switch (k) {
    case 0:  // Top
        return (put << 8) & 0xffff'ffff'ffff'ff00;
    case 1:  // Top-right
        return (put << 7) & 0x7f7f'7f7f'7f7f'7f00;
    case 2:  // Right
        return (put >> 1) & 0x7f7f'7f7f'7f7f'7f7f;
    case 3:  // Bottom-right
        return (put >> 9) & 0x007f'7f7f'7f7f'7f7f;
    case 4:  // Bottom
        return (put >> 8) & 0x00ff'ffff'ffff'ffff;
    case 5:  // Bottom-left
        return (put >> 7) & 0x00fe'fefe'fefe'fefe;
    case 6:  // Left
        return (put << 1) & 0xfefe'fefe'fefe'fefe;
    case 7:  // Top-left
        return (put << 9) & 0xfefe'fefe'fefe'fe00;
    }
    return 0;
}

bool isAbleToPut(uint64_t put, uint64_t b0, uint64_t b1) {
    const uint64_t legal = makeLegalBoard(b0, b1);
    return (put & legal) == put;
}

bool isGameSet(uint64_t b0, uint64_t b1) {
    const uint64_t l0 = makeLegalBoard(b0, b1);
    const uint64_t l1 = makeLegalBoard(b1, b0);
    return (l0 == 0) && (l1 == 0);
}

int bitCount(uint64_t bits) {
    bits = bits - ((bits >> 1) & 0x5555555555555555);
    bits = (bits & 0x3333333333333333) + ((bits >> 2) & 0x3333333333333333);
    return (((bits + (bits >> 4)) & 0xF0F0F0F0F0F0F0F) * 0x101010101010101) >> 56;
}
