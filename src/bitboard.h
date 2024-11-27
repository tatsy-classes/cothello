#ifndef _OTHELLO_BITBOARD_H_
#define _OTHELLO_BITBOARD_H_

#include <cstdint>
#include <string>
#include <vector>
#include <tuple>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

static constexpr int NUM_CELLS = 64;

uint64_t coordinatesToBits(const std::string &xy);
std::array<int, NUM_CELLS> bitsToBoard(uint64_t bits);
uint64_t makeLegalBoard(uint64_t b0, uint64_t b1);
std::tuple<uint64_t, uint64_t> reverse(uint64_t put, uint64_t b0, uint64_t b1);
uint64_t transfer(uint64_t put, int k);
bool isAbleToPut(uint64_t put, uint64_t b0, uint64_t b1);
bool isGameSet(uint64_t b0, uint64_t b1);
int bitCount(uint64_t bits);

#endif  // _OTHELLO_BITBOARD_H_
