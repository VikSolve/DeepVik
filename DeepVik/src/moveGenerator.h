#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H
#pragma GCC optimize "Ofast,unroll-loops,omit-frame-pointer,inline"
#pragma GCC target("popcnt")
#include <bits/stdc++.h>
#include "myBot.h"
std::string translate(uint32_t x);
std::vector<std::string> cap(Game state, bool tura, uint32_t nr, bool king);
std::vector<std::string> generateMoves(Game state, bool tura);
#endif
