#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>

uint64_t coordinatesToBits(const std::string &xy);
std::vector<int> bitsToBoard(uint64_t bits);
uint64_t makeLegalBoard(uint64_t b0, uint64_t b1);
std::pair<uint64_t, uint64_t> reverse(uint64_t put, uint64_t b0, uint64_t b1);
uint64_t transfer(uint64_t put, int k);
int bitCount(uint64_t bits);

#endif  // BITBOARD_H
