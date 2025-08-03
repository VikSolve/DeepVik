#ifndef MYBOT_H
#define MYBOT_H

#pragma GCC optimize "Ofast,unroll-loops,omit-frame-pointer,inline"
#pragma GCC target("popcnt")

#include <bits/stdc++.h>
#include <sys/time.h>

#define INLINE __attribute__((always_inline))inline
#define NOINLINE __attribute__((noinline))
#define LIKELY(x)   (__builtin_expect(!!(x), 1))
#define UNLIKELY(x) (__builtin_expect(!!(x), 0))
#define ABS(x) ((x) < 0 ? -(x) : (x))

using namespace std;

extern"C"{
extern int gettimeofday(timeval*,void*);
}

INLINE uint64_t getRTime(){
    timeval t;
    gettimeofday(&t,0);
    return t.tv_sec*1000000UL+t.tv_usec;
}


class Game{
public:
    uint32_t red_pawn;
    uint32_t black_pawn;
    uint32_t red_king;
    uint32_t black_king;

    Game();
    void reset();
    INLINE void black_pawn_move(const uint32_t x, const uint32_t y);
    INLINE void red_pawn_move(const uint32_t x, const uint32_t y);
    INLINE void black_king_move(const uint32_t x, const uint32_t y);
    INLINE void red_king_move(const uint32_t x, const uint32_t y);
    INLINE void black_pawn_attack(const uint32_t x, const uint32_t y);
    INLINE void red_pawn_attack(const uint32_t x, const uint32_t y);
    INLINE void black_king_attack(const uint32_t x, const uint32_t y);
    INLINE void red_king_attack(const uint32_t x, const uint32_t y);
};

struct Node {
    uint64_t key   = 0;
    int16_t score = 0;
    uint16_t hashMove = 0;
    uint8_t depth = 0;
    uint8_t flag  = 0;
};

enum : uint8_t {
    FLAG_UPPER = 0,
    FLAG_LOWER = 1,
    FLAG_EXACT = 2
};

class FastTT {
private:
    vector<Node> table;
public:
    static constexpr uint32_t TABLE_SIZE = (1u << 22);
    static constexpr uint32_t SIZE_MASK  = TABLE_SIZE - 1;

    FastTT();
    INLINE uint64_t hash(const Game &g, bool tura) const;
    INLINE Node find(const Game &g, bool tura) const;
    INLINE void insert(const Game &g, uint8_t depth, int16_t score, uint16_t hashMove, uint8_t flag, bool tura);
    void delete_root(const Game &g, bool tura);
    void clear();
};


extern FastTT tt;
extern uint8_t MAX_DEPTH;
extern array<int16_t, 12> buff, saved;
extern int16_t best_result;
extern uint64_t t0;
extern bool stop_iterating;
extern uint16_t timeCount;
extern uint16_t killerMove[60][2];

string move_recovery(const Game &state, bool my_color);
pair<int, string> BestMove(Game &state, int depthLimit, bool my_color);
void update_board(Game &state, vector<string> &board);
template<bool Tura> int16_t dfs(const Game &state, uint8_t depth, int16_t alpha, int16_t beta);
template<bool Tura, bool King> int16_t dfs_pom(const Game &state, uint8_t depth, uint32_t nr, int16_t alpha, int16_t beta);
template<bool Tura> INLINE bool applyKillerMove(Game temp, uint32_t all, int16_t &res, int16_t &alpha, int16_t &beta, uint8_t depth, uint16_t &hashMove);


INLINE void Game::black_pawn_move(const uint32_t x, const uint32_t y){
    uint32_t y_clc = (1u << y);
    black_pawn ^= (1u << x);
    if(UNLIKELY((y >> 2u) == 7)) black_king ^= y_clc;
    else black_pawn ^= y_clc;
}
INLINE void Game::red_pawn_move(const uint32_t x, const uint32_t y){
    uint32_t y_clc = (1u << y);
    red_pawn ^= (1u << x);
    if(UNLIKELY((y >> 2u) == 0)) red_king ^= y_clc;
    else red_pawn ^= y_clc;
}
INLINE void Game::black_king_move(const uint32_t x, const uint32_t y){
    uint32_t y_clc = (1u << y);
    black_king ^= (1u << x);
    black_king ^= y_clc;
}
INLINE void Game::red_king_move(const uint32_t x, const uint32_t y){
    uint32_t y_clc = (1u << y);
    red_king ^= (1u << x);
    red_king ^= y_clc;
}
INLINE void Game::black_pawn_attack(const uint32_t x, const uint32_t y){
    uint32_t mid = 1u << (x + 3 + ((x >> 2u) & 1u) + (x + 9u == y));
    uint32_t y_clc = (1u << y);
    if(UNLIKELY((y >> 2u) == 7)) black_king ^= y_clc;
    else black_pawn ^= y_clc;
    black_pawn ^= (1u << x);
    red_pawn ^= red_pawn & mid;
    red_king ^= red_king & mid;
}
INLINE void Game::red_pawn_attack(const uint32_t x, const uint32_t y){
    uint32_t mid = 1u << (x - 5 + ((x >> 2u) & 1u) + (x == y + 7u));
    uint32_t y_clc = (1u << y);
    if(UNLIKELY((y >> 2u) == 0)) red_king ^= y_clc;
    else red_pawn ^= y_clc;
    red_pawn ^= (1u << x);
    black_pawn ^= black_pawn & mid;
    black_king ^= black_king & mid;
}

INLINE void Game::black_king_attack(const uint32_t x, const uint32_t y){
    uint32_t mid = x + ((x >> 2u) & 1u);
    if(y > x) mid += (x + 9u == y) + 3;
    else mid -= 5u - (x == y + 7u);
    mid = 1u << mid;
    uint32_t y_clc = (1u << y);
    black_king ^= (1u << x);
    black_king ^= y_clc;
    red_pawn ^= red_pawn & mid;
    red_king ^= red_king & mid;
}

INLINE void Game::red_king_attack(const uint32_t x, const uint32_t y){
    uint32_t mid = x + ((x >> 2u) & 1u);
    if(y > x) mid += (x + 9u == y) + 3;
    else mid -= 5u - (x == y + 7u);
    mid = 1u << mid;
    uint32_t y_clc = (1u << y);
    red_king ^= (1u << x);
    red_king ^= y_clc;
    black_pawn ^= black_pawn & mid;
    black_king ^= black_king & mid;
}

INLINE uint64_t FastTT::hash(const Game &g, bool tura) const {
    uint64_t h = 11816180607513830921ULL * g.red_pawn
                 ^ 17816992196493881569ULL * g.black_pawn
                 ^ 12329556594590792737ULL * g.red_king
                 ^ 10419611012853566743ULL * g.black_king
                 ^ 0x9e3779b97f4a7c15ULL * tura;
    h ^= (h >> 21) ^ (h >> 42);
    return h;
}

INLINE Node FastTT::find(const Game &g, bool tura) const {
    uint64_t key = hash(g, tura);
    const Node &n = table[key & SIZE_MASK];
    static constexpr Node EMPTY{};
    return (n.key == key) ? n : EMPTY;
}

INLINE void FastTT::insert(const Game &g, uint8_t depth, int16_t score, uint16_t hashMove, uint8_t flag, bool tura) {
    uint64_t key = hash(g, tura);
    Node &n = table[key & SIZE_MASK];
    if (n.key != key || depth >= n.depth) {
        n.key = key;
        n.score = score;
        n.hashMove = hashMove;
        n.depth = depth;
        n.flag = flag;
    }
}

template<bool Tura, bool King>
int16_t dfs_pom(const Game &state, uint8_t depth, uint32_t nr, int16_t alpha, int16_t beta){
    if(UNLIKELY(depth == 0)){
        buff[buff[11]++] = nr;
    }
    Game temp = state;
    uint32_t red = temp.red_pawn | temp.red_king;
    uint32_t black = temp.black_pawn | temp.black_king;
    uint32_t all = black | red;
    uint32_t pm = 1u << nr;
    uint32_t attack_left, attack_right, attack_back_left, attack_back_right;
    bool mv = true;
    if constexpr (!Tura){
        attack_left = (black & 0x00EEEEEE) & (((red & 0x0F0F0F0F) >> 4) | ((red & 0xF0F0F0F0) >> 3)) & ((~all) >> 7);
        attack_right = (black & 0x00777777) & (((red & 0x0F0F0F0F) >> 5) | ((red & 0xF0F0F0F0) >> 4)) & ((~all) >> 9);
        attack_back_left = (temp.black_king & 0xEEEEEE00) & (((red & 0x0F0F0F0F) << 4) | ((red & 0xF0F0F0F0) << 5)) & (~(all) << 9);
        attack_back_right = (temp.black_king & 0x77777700) & (((red & 0x0F0F0F0F) << 3) | ((red & 0xF0F0F0F0) << 4)) & (~(all) << 7);
        int16_t res = -10000;
        if constexpr(!King){
            if(temp.black_pawn & pm){
                if(pm & attack_left){
                    mv = false;
                    temp.black_pawn_attack(nr, nr + 7u);
                    int16_t child = dfs_pom<false, false>(temp, depth, nr + 7u, alpha, beta);
                    if(res < child){
                        res = child;
                        if(child > alpha){
                            alpha = child;
                            if(alpha >= beta) return res;
                        }
                    }
                    temp = state;
                }
                if(pm & attack_right){
                    mv = false;
                    temp.black_pawn_attack(nr, nr + 9u);
                    int16_t child = dfs_pom<false, false>(temp, depth, nr + 9u, alpha, beta);
                    if(res < child){
                        res = child;
                        if(child > alpha){
                            alpha = child;
                            if(alpha >= beta) return res;
                        }
                    }
                    temp = state;
                }
            }
        }
        else{
            if(pm & attack_left){
                mv = false;
                temp.black_king_attack(nr, nr + 7u);
                int16_t child = dfs_pom<false, true>(temp, depth, nr + 7u, alpha, beta);
                if(res < child){
                    res = child;
                    if(child > alpha){
                        alpha = child;
                        if(alpha >= beta) return res;
                    }
                }
                temp = state;
            }
            if(pm & attack_right){
                mv = false;
                temp.black_king_attack(nr, nr + 9u);
                int16_t child = dfs_pom<false, true>(temp, depth, nr + 9u, alpha, beta);
                if(res < child){
                    res = child;
                    if(child > alpha){
                        alpha = child;
                        if(alpha >= beta) return res;
                    }
                }
                temp = state;
            }
            if(pm & attack_back_left){
                mv = false;
                temp.black_king_attack(nr, nr - 9u);
                int16_t child = dfs_pom<false, true>(temp, depth, nr - 9u, alpha, beta);
                if(res < child){
                    res = child;
                    if(child > alpha){
                        alpha = child;
                        if(alpha >= beta) return res;
                    }
                }
                temp = state;
            }
            if(pm & attack_back_right){
                mv = false;
                temp.black_king_attack(nr, nr - 7u);
                int16_t child = dfs_pom<false, true>(temp, depth, nr - 7u, alpha, beta);
                if(res < child){
                    res = child;
                    if(child > alpha){
                        alpha = child;
                        if(alpha >= beta) return res;
                    }
                }
                temp = state;
            }
        }
        if(mv){
            if(UNLIKELY(depth == 0)){
                array<int16_t, 12> copy = buff;
                buff[11] = 0;
                int16_t curr = dfs<true>(temp, depth + 1, alpha, beta);
                if(curr > best_result){
                    best_result = curr;
                    saved = copy;
                }
                buff = copy;
                buff[11]--;
                return curr;
            }
            else return dfs<true>(temp, depth + 1, alpha, beta);
        }
        else{
            if(UNLIKELY(depth == 0)) buff[11]--;
            return res;
        }
    }
    else{
        attack_left = (red & 0xEEEEEE00) & (((black & 0x0F0F0F0F) << 4) | ((black & 0xF0F0F0F0) << 5)) & (~(all) << 9);
        attack_right = (red & 0x77777700) & (((black & 0x0F0F0F0F) << 3) | ((black & 0xF0F0F0F0) << 4)) & (~(all) << 7);
        attack_back_left = (temp.red_king & 0x00EEEEEE) & (((black & 0x0F0F0F0F) >> 4) | ((black & 0xF0F0F0F0) >> 3)) & ((~all) >> 7);
        attack_back_right = (temp.red_king & 0x00777777) & (((black & 0x0F0F0F0F) >> 5) | ((black & 0xF0F0F0F0) >> 4)) & ((~all) >> 9);
        int16_t res = 10000;
        if constexpr(!King){
            if(temp.red_pawn & pm){
                if(pm & attack_left){
                    mv = false;
                    temp.red_pawn_attack(nr, nr - 9u);
                    int16_t child = dfs_pom<true, false>(temp, depth, nr - 9u, alpha, beta);
                    if(child < res){
                        res = child;
                        if(child < beta){
                            beta = child;
                            if(alpha >= beta) return res;
                        }
                    }
                    temp = state;
                }
                if(pm & attack_right){
                    mv = false;
                    temp.red_pawn_attack(nr, nr - 7u);
                    int16_t child = dfs_pom<true, false>(temp, depth, nr - 7u, alpha, beta);
                    if(child < res){
                        res = child;
                        if(child < beta){
                            beta = child;
                            if(alpha >= beta) return res;
                        }
                    }
                    temp = state;
                }
            }
        }
        else{
            if(pm & attack_left){
                mv = false;
                temp.red_king_attack(nr, nr - 9u);
                int16_t child = dfs_pom<true, true>(temp, depth, nr - 9u, alpha, beta);
                if(child < res){
                    res = child;
                    if(child < beta){
                        beta = child;
                        if(alpha >= beta) return res;
                    }
                }
                temp = state;
            }
            if(pm & attack_right){
                mv = false;
                temp.red_king_attack(nr, nr - 7u);
                int16_t child = dfs_pom<true, true>(temp, depth, nr - 7u, alpha, beta);
                if(child < res){
                    res = child;
                    if(child < beta){
                        beta = child;
                        if(alpha >= beta) return res;
                    }
                }
                temp = state;
            }

            if(pm & attack_back_left){
                mv = false;
                temp.red_king_attack(nr, nr + 7u);
                int16_t child = dfs_pom<true, true>(temp, depth, nr + 7u, alpha, beta);
                if(child < res){
                    res = child;
                    if(child < beta){
                        beta = child;
                        if(alpha >= beta) return res;
                    }
                }
                temp = state;
            }
            if(pm & attack_back_right){
                mv = false;
                temp.red_king_attack(nr, nr + 9u);
                int16_t child = dfs_pom<true, true>(temp, depth, nr + 9u, alpha, beta);
                if(child < res){
                    res = child;
                    if(child < beta){
                        beta = child;
                        if(alpha >= beta) return res;
                    }
                }
                temp = state;
            }
        }
        if(mv){
            if(UNLIKELY(depth == 0)){
                array<int16_t, 12> copy = buff;
                buff[11] = 0;
                int16_t curr = dfs<false>(temp, depth + 1, alpha, beta);
                if(curr < best_result){
                    best_result = curr;
                    saved = copy;
                }
                buff = copy;
                buff[11]--;
                return curr;
            }
            else return dfs<false>(temp, depth + 1, alpha, beta);
        }
        else{
            if(UNLIKELY(depth == 0)) buff[11]--;
            return res;
        }
    }
}

template<bool Tura>
INLINE bool applyKillerMove(Game temp, uint32_t all, int16_t &res, int16_t &alpha, int16_t &beta, uint8_t depth, uint16_t &hashMove){
    uint16_t killmove = killerMove[depth][0];
    if constexpr(!Tura){
        if(killmove && killmove != hashMove){
            uint32_t from = killmove >> 8;
            uint32_t to = (killmove >> 2) & 31;
            int16_t child = -32767;
            if(!(all & (1u << to)) && ((temp.black_pawn || temp.black_king) & (1u << from))){
                if((killmove & 1) && (temp.black_king & (1 << from))){
                    temp.black_king_move(from, to);
                    child = dfs<true>(temp, depth + 1, alpha, beta);
                }
                else if(!(killmove & 1) && (temp.black_pawn & (1u << from))){
                    temp.black_pawn_move(from, to);
                    child = dfs<true>(temp, depth + 1, alpha, beta);
                }
                if(res < child){
                    res = child;
                    hashMove = killmove;
                    if(alpha < child){
                        alpha = child;
                        if(alpha >= beta) return true;
                    }
                }
            }
        }
        killmove = killerMove[depth][1];
        if(killmove && killmove != killerMove[depth][0]){
            uint32_t from = killmove >> 8;
            uint32_t to = (killmove >> 2) & 31;
            int16_t child = -32767;
            if(!(all & (1u << to)) && ((temp.black_pawn || temp.black_king) & (1u << from))){
                if((killmove & 1) && (temp.black_king & (1 << from))){
                    temp.black_king_move(from, to);
                    child = dfs<true>(temp, depth + 1, alpha, beta);
                }
                else if(!(killmove & 1) && (temp.black_pawn & (1u << from))){
                    temp.black_pawn_move(from, to);
                    child = dfs<true>(temp, depth + 1, alpha, beta);
                }
                if(res < child){
                    res = child;
                    hashMove = killmove;
                    killerMove[depth][1] = killerMove[depth][0];
                    killerMove[depth][0] = killmove;
                    if(alpha < child){
                        alpha = child;
                        if(alpha >= beta) return true;
                    }
                }
            }
        }
    }
    else{
        if(killmove && killmove != hashMove){
            uint32_t from = killmove >> 8;
            uint32_t to = (killmove >> 2) & 31;
            int16_t child = 32767;
            if(!(all & (1u << to)) && ((temp.red_pawn || temp.red_king) & (1u << from))){
                if((killmove & 1) && (temp.red_king & (1 << from))){
                    temp.red_king_move(from, to);
                    child = dfs<false>(temp, depth + 1, alpha, beta);
                }
                else if(!(killmove & 1) && (temp.red_pawn & (1u << from))){
                    temp.red_pawn_move(from, to);
                    child = dfs<false>(temp, depth + 1, alpha, beta);
                }
                if(res > child){
                    res = child;
                    hashMove = killmove;
                    if(beta > child){
                        beta = child;
                        if(alpha >= beta) return true;
                    }
                }
            }
        }
        killmove = killerMove[depth][1];
        if(killmove && killmove != killerMove[depth][0]){
            uint32_t from = killmove >> 8;
            uint32_t to = (killmove >> 2) & 31;
            int16_t child = 32767;
            if(!(all & (1u << to)) && ((temp.red_pawn || temp.red_king) & (1u << from))){
                if((killmove & 1) && (temp.red_king & (1 << from))){
                    temp.red_king_move(from, to);
                    child = dfs<false>(temp, depth + 1, alpha, beta);
                }
                else if(!(killmove & 1) && (temp.red_pawn & (1u << from))){
                    temp.red_pawn_move(from, to);
                    child = dfs<false>(temp, depth + 1, alpha, beta);
                }
                if(res > child){
                    res = child;
                    hashMove = killmove;
                    killerMove[depth][1] = killerMove[depth][0];
                    killerMove[depth][0] = killmove;
                    if(beta > child){
                        beta = child;
                        if(alpha >= beta) return true;
                    }
                }
            }
        }
    }
    return false;
}

template<bool Tura>
int16_t dfs(const Game &state, uint8_t depth, int16_t alpha, int16_t beta){
    if(UNLIKELY(stop_iterating)) return 0;
    if(depth == MAX_DEPTH){
        int16_t black = __builtin_popcount(state.black_pawn) + (__builtin_popcount(state.black_king) << 1);
        int16_t red = __builtin_popcount(state.red_pawn) + (__builtin_popcount(state.red_king) << 1);
        black <<= 5;
        red <<= 5;
        uint32_t bits_black = state.black_pawn | state.black_king;
        uint32_t bits_red = state.red_pawn | state.red_king;
        //0000 1110 0221 1320 0231 1220 0111 0000
        black += __builtin_popcount(bits_black & 270336);
        //promotion zone
        black += __builtin_popcount(state.black_pawn & 2130706686) << 1;
        //center band
        black += __builtin_popcount(bits_black & 1044480);
        //most
        black += ((int16_t)((state.black_pawn & 10u) == 10u)) << 2;
        //triangles
        int16_t triangles = 0;
        triangles += __builtin_popcount((bits_black & 0x00077777) & ((bits_black & 0x000EEEEE) << 1) & (((bits_black & 0x00707070) << 4) | ((bits_black & 0x0E0E0E00) << 5)));
        //0000 1110 0221 1320 0231 1220 0111 0000
        red += __builtin_popcount(bits_red & 270336);
        //promotion zone
        red += __builtin_popcount(state.red_pawn & 2130706686) << 1;
        //center band
        red += __builtin_popcount(bits_red & 1044480);
        //most
        red += ((int16_t)((state.red_pawn & 1342177280u) == 1342177280u)) << 2;
        //triangles
        triangles -= __builtin_popcount(((bits_red & 0x77777000) >> 1) & (bits_red & 0xEEEEE000) & (((bits_red & 0x0E0E0E00) >> 4) | ((bits_red & 0x00707070) >> 5)));


        //wyrównanie
        triangles += ((int16_t)(__builtin_popcount(bits_black & 0x3333377F) - __builtin_popcount(bits_red & 0x77333333)));
        triangles -= ((int16_t)(__builtin_popcount(bits_red & 0xFEECCCCC) - __builtin_popcount(bits_black & 0xCCCCCCEE)));
        int16_t res = black - red + (triangles * 3);
        //agresja
        return res - (((res >> 6) + ((res >> 15) & 1) ) * 8);
    }
    timeCount++;
    if(UNLIKELY((timeCount & 127) == 0)){
        uint64_t t1 = getRTime();
        if(UNLIKELY(t1 - t0 > 97500)){
            int ms = (t1 - t0)/1000;
            cerr << "Czas: " << ms << " ms, MAX_DEPTH = "<<(int)MAX_DEPTH-1<<"\n";
            stop_iterating = true;
            return 0;
        }
    }
    int16_t origAlpha = alpha;
    int16_t origBeta  = beta;
    Node node = tt.find(state, Tura);
    uint8_t rem = MAX_DEPTH - depth;
    uint16_t hashMove = 0;
    /* format hashmove i killerMove:
    * hashmove >> 8, start position
    * (hashmove >> 2) & 31, end position
    * hashmove & 2   if equal 1 then attack, else normal move
    * hashmove & 1   if equal 1 then king else pawn */
    if(node.key != 0){
        if(node.depth >= rem && depth){
            if(node.flag == FLAG_EXACT) return node.score;
            else if(node.flag == FLAG_LOWER) alpha = max(alpha, node.score);
            else beta = min(beta, node.score);
            if(alpha >= beta) return node.score;
        }
        hashMove = node.hashMove;
    }
    Game temp = state;
    uint32_t red = temp.red_king | temp.red_pawn;
    uint32_t black = temp.black_king | temp.black_pawn;
    uint32_t all = red | black;
    uint32_t attack_left, attack_right, attack_back_left, attack_back_right;
    if constexpr(!Tura){
        int16_t res = -10000 + depth;
        uint32_t bits;
        if(hashMove){
            uint32_t from = hashMove >> 8;
            uint32_t to = (hashMove >> 2) & 31;
            int16_t child;
            if(hashMove & 2){
                if(hashMove & 1){
                    temp.black_king_attack(from, to);
                    child = dfs_pom<false, true>(temp, depth, to, alpha, beta);
                }
                else{
                    temp.black_pawn_attack(from, to);
                    child = dfs_pom<false, false>(temp, depth, to, alpha, beta);
                }

            }
            else{
                if(hashMove & 1) temp.black_king_move(from, to);
                else temp.black_pawn_move(from, to);
                child = dfs<true>(temp, depth + 1, alpha, beta);
            }
            if(res < child){
                res = child;
                if(alpha < child){
                    alpha = child;
                    if(alpha >= beta){
                        if(!(hashMove & 2)){
                            killerMove[depth][1] = killerMove[depth][0];
                            killerMove[depth][0] = hashMove;
                        }
                        goto DONE1;
                    }
                }
            }
            temp = state;
        }
        attack_left = (black & 0x00EEEEEE) & (((red & 0x0F0F0F0F) >> 4) | ((red & 0xF0F0F0F0) >> 3)) & ((~all) >> 7);
        attack_right = (black & 0x00777777) & (((red & 0x0F0F0F0F) >> 5) | ((red & 0xF0F0F0F0) >> 4)) & ((~all) >> 9);
        attack_back_left = (temp.black_king & 0xEEEEEE00) & (((red & 0x0F0F0F0F) << 4) | ((red & 0xF0F0F0F0) << 5)) & (~(all) << 9);
        attack_back_right = (temp.black_king & 0x77777700) & (((red & 0x0F0F0F0F) << 3) | ((red & 0xF0F0F0F0) << 4)) & (~(all) << 7);
        if(!(attack_left | attack_right | attack_back_left| attack_back_right)){
            if(applyKillerMove<false>(temp, all, res, alpha, beta, depth, hashMove)){
                goto DONE1;
            }
            bits = temp.black_pawn & 0x0FFFFFFF;
            while (bits) {
                uint32_t i = __builtin_ctz(bits);
                uint16_t cond = i & 7u;
                uint32_t pr = i + ((i >> 2u) & 1u) + 3u;
                if(cond && !(all & (1u << pr))){
                    temp.black_pawn_move(i, pr);
                    int16_t child = dfs<true>(temp, depth + 1, alpha, beta);
                    if(res < child){
                        res = child;
                        hashMove = ((i << 6) | pr) << 2;
                        if(alpha < child){
                            alpha = child;
                            if(alpha >= beta){
                                killerMove[depth][1] = killerMove[depth][0];
                                killerMove[depth][0] = hashMove;
                                goto DONE1;
                            }
                        }
                    }
                    temp = state;
                }
                if((cond ^ 7) && !(all & (1u << (pr + 1)))){
                    temp.black_pawn_move(i, pr + 1);
                    int16_t child = dfs<true>(temp, depth + 1, alpha, beta);
                    if(res < child){
                        res = child;
                        hashMove = ((i << 6) | (pr + 1)) << 2;
                        if(alpha < child){
                            alpha = child;
                            if(alpha >= beta){
                                killerMove[depth][1] = killerMove[depth][0];
                                killerMove[depth][0] = hashMove;
                                goto DONE1;
                            }
                        }
                    }
                    temp = state;
                }
                bits &= (bits - 1u);
            }
            bits = temp.black_king;
            while (bits) {
                uint32_t i = __builtin_ctz(bits);
                uint16_t cond = i & 7u;
                uint32_t pr = i + ((i >> 2u) & 1u) + 3u;
                if(i < 28){
                    if(cond && !(all & (1u << pr))){
                        temp.black_king_move(i, pr);
                        int16_t child = dfs<true>(temp, depth + 1, alpha, beta);
                        if(res < child){
                            res = child;
                            hashMove = (((i << 6) | pr) << 2) | 1;
                            if(alpha < child){
                                alpha = child;
                                if(alpha >= beta){
                                    killerMove[depth][1] = killerMove[depth][0];
                                    killerMove[depth][0] = hashMove;
                                    goto DONE1;
                                }
                            }
                        }
                        temp = state;
                    }
                    if((cond ^ 7) && !(all & (1u << (pr + 1)))){
                        temp.black_king_move(i, pr + 1);
                        int16_t child = dfs<true>(temp, depth + 1, alpha, beta);
                        if(res < child){
                            res = child;
                            hashMove = (((i << 6) | (pr + 1)) << 2) | 1;
                            if(alpha < child){
                                alpha = child;
                                if(alpha >= beta){
                                    killerMove[depth][1] = killerMove[depth][0];
                                    killerMove[depth][0] = hashMove;
                                    goto DONE1;
                                }
                            }
                        }
                        temp = state;
                    }
                }
                if(i > 3){
                    if(cond && !(all & (1u << (pr - 8u)))){
                        temp.black_king_move(i, pr - 8u);
                        int16_t child = dfs<true>(temp, depth + 1, alpha, beta);
                        if(res < child){
                            res = child;
                            hashMove = (((i << 6) | (pr - 8u)) << 2) | 1;
                            if(alpha < child){
                                alpha = child;
                                if(alpha >= beta){
                                    killerMove[depth][1] = killerMove[depth][0];
                                    killerMove[depth][0] = hashMove;
                                    goto DONE1;
                                }
                            }
                        }
                        temp = state;
                    }
                    if((cond ^ 7) && !(all & (1u << (pr - 7u)))){
                        temp.black_king_move(i, pr - 7u);
                        int16_t child = dfs<true>(temp, depth + 1, alpha, beta);
                        if(res < child){
                            res = child;
                            hashMove = (((i << 6) | (pr - 7)) << 2) | 1;
                            if(alpha < child){
                                alpha = child;
                                if(alpha >= beta){
                                    killerMove[depth][1] = killerMove[depth][0];
                                    killerMove[depth][0] = hashMove;
                                    goto DONE1;
                                }
                            }
                        }
                        temp = state;
                    }
                }
                bits &= (bits - 1u);
            }
        }
        else{
            bits = temp.black_pawn & (attack_left | attack_right);
            while (bits) {
                uint32_t i = __builtin_ctz(bits);
                uint32_t mp = 1u << i;
                if(mp & attack_left){
                    temp.black_pawn_attack(i, i + 7);
                    int16_t child = dfs_pom<false, false>(temp, depth, i + 7u, alpha, beta);
                    if(res < child){
                        res = child;
                        hashMove = (((i << 6) | (i + 7)) << 2) | 2;
                        if(alpha < child){
                            alpha = child;
                            if(alpha >= beta){
                                goto DONE1;
                            }
                        }
                    }
                    temp = state;
                }
                if(mp & attack_right){
                    temp.black_pawn_attack(i, i + 9u);
                    int16_t child = dfs_pom<false, false>(temp, depth, i + 9u, alpha, beta);
                    if(res < child){
                        res = child;
                        hashMove = (((i << 6) | (i + 9)) << 2) | 2;
                        if(alpha < child){
                            alpha = child;
                            if(alpha >= beta){
                                goto DONE1;
                            }
                        }
                    }
                    temp = state;
                }
                bits &= (bits - 1u);
            }
            bits = temp.black_king & (attack_left | attack_right | attack_back_left | attack_back_right);
            while (bits) {
                uint32_t i = __builtin_ctz(bits);
                uint32_t mp = 1u << i;
                if(mp & attack_left){
                    temp.black_king_attack(i, i + 7u);
                    int16_t child = dfs_pom<false, true>(temp, depth, i + 7u, alpha, beta);
                    if(res < child){
                        res = child;
                        hashMove = (((i << 6) | (i + 7)) << 2) | 3;
                        if(alpha < child){
                            alpha = child;
                            if(alpha >= beta){
                                goto DONE1;
                            }
                        }
                    }
                    temp = state;
                }
                if(mp & attack_right){
                    temp.black_king_attack(i, i + 9u);
                    int16_t child = dfs_pom<false, true>(temp, depth, i + 9u, alpha, beta);
                    if(res < child){
                        res = child;
                        hashMove = (((i << 6) | (i + 9)) << 2) | 3;
                        if(alpha < child){
                            alpha = child;
                            if(alpha >= beta){
                                goto DONE1;
                            }
                        }
                    }
                    temp = state;
                }

                if(mp & attack_back_left){
                    temp.black_king_attack(i, i - 9u);
                    int16_t child = dfs_pom<false, true>(temp, depth, i - 9u, alpha, beta);
                    if(res < child){
                        res = child;
                        hashMove = (((i << 6) | (i - 9)) << 2) | 3;
                        if(alpha < child){
                            alpha = child;
                            if(alpha >= beta){
                                goto DONE1;
                            }
                        }
                    }
                    temp = state;
                }
                if(mp & attack_back_right){
                    temp.black_king_attack(i, i - 7u);
                    int16_t child = dfs_pom<false, true>(temp, depth, i - 7u, alpha, beta);
                    if(res < child){
                        res = child;
                        hashMove = (((i << 6) | (i - 7)) << 2) | 3;
                        if(alpha < child){
                            alpha = child;
                            if(alpha >= beta){
                                goto DONE1;
                            }
                        }
                    }
                    temp = state;
                }

                bits &= (bits - 1u);
            }
        }
    DONE1:
        if(LIKELY(!stop_iterating)){
            uint8_t flag;
            if(res <= origAlpha) flag = FLAG_UPPER;
            else if(res >= origBeta) flag = FLAG_LOWER;
            else flag = FLAG_EXACT;
            tt.insert(state, rem, res, hashMove, flag, Tura);
        }
        return res;
    }
    else{
        int16_t res = 10000 - depth;
        uint32_t bits;
        if(hashMove){
            uint32_t from = hashMove >> 8;
            uint32_t to = (hashMove >> 2) & 31;
            int16_t child;
            if(hashMove & 2){
                if(hashMove & 1){
                    temp.red_king_attack(from, to);
                    child = dfs_pom<true, true>(temp, depth, to, alpha, beta);
                }
                else{
                    temp.red_pawn_attack(from, to);
                    child = dfs_pom<true, false>(temp, depth, to, alpha, beta);
                }

            }
            else{
                if(hashMove & 1) temp.red_king_move(from, to);
                else temp.red_pawn_move(from, to);
                child = dfs<false>(temp, depth + 1, alpha, beta);
            }
            if(res > child){
                res = child;
                if(beta > child){
                    beta = child;
                    if(alpha >= beta){
                        if(!(hashMove & 2)){
                            killerMove[depth][1] = killerMove[depth][0];
                            killerMove[depth][0] = hashMove;
                        }
                        goto DONE2;
                    }
                }
            }
            temp = state;
        }
        attack_left = (red & 0xEEEEEE00) & (((black & 0x0F0F0F0F) << 4) | ((black & 0xF0F0F0F0) << 5)) & (~(all) << 9);
        attack_right = (red & 0x77777700) & (((black & 0x0F0F0F0F) << 3) | ((black & 0xF0F0F0F0) << 4)) & (~(all) << 7);
        attack_back_left = (temp.red_king & 0x00EEEEEE) & (((black & 0x0F0F0F0F) >> 4) | ((black & 0xF0F0F0F0) >> 3)) & ((~all) >> 7);
        attack_back_right = (temp.red_king & 0x00777777) & (((black & 0x0F0F0F0F) >> 5) | ((black & 0xF0F0F0F0) >> 4)) & ((~all) >> 9);
        if(!(attack_left | attack_right | attack_back_left| attack_back_right)){
            if(applyKillerMove<true>(temp, all, res, alpha, beta, depth, hashMove)){
                goto DONE2;
            }
            temp = state;
            bits = temp.red_pawn & 0xFFFFFFF0;
            while (bits) {
                uint32_t i = __builtin_ctz(bits);
                uint16_t cond = i & 7u;
                uint32_t pr = i + ((i >> 2u) & 1u) - 5u;
                if(cond && !(all & (1u << pr))){
                    temp.red_pawn_move(i, pr);
                    int16_t child = dfs<false>(temp, depth + 1, alpha, beta);
                    if(res > child){
                        res = child;
                        hashMove = ((i << 6) | pr) << 2;
                        if(beta > child){
                            beta = child;
                            if(alpha >= beta){
                                killerMove[depth][1] = killerMove[depth][0];
                                killerMove[depth][0] = hashMove;
                                goto DONE2;
                            }
                        }
                    }
                    temp = state;
                }
                if((cond ^ 7) && !(all & (1u << (pr + 1)))){
                    temp.red_pawn_move(i, pr + 1);
                    int16_t child = dfs<false>(temp, depth + 1, alpha, beta);
                    if(res > child){
                        res = child;
                        hashMove = ((i << 6) | (pr + 1)) << 2;
                        if(beta > child){
                            beta = child;
                            if(alpha >= beta){
                                killerMove[depth][1] = killerMove[depth][0];
                                killerMove[depth][0] = hashMove;
                                goto DONE2;
                            }
                        }
                    }
                    temp = state;
                }
                bits &= (bits - 1u);
            }
            bits = temp.red_king;
            while (bits) {
                uint32_t i = __builtin_ctz(bits);
                uint16_t cond = i & 7u;
                uint32_t pr = i + ((i >> 2u) & 1u) + 3u;
                if(i < 28){
                    if(cond && !(all & (1u << pr))){
                        temp.red_king_move(i, pr);
                        int16_t child = dfs<false>(temp, depth + 1, alpha, beta);
                        if(res > child){
                            res = child;
                            hashMove = (((i << 6) | pr) << 2) | 1;
                            if(beta > child){
                                beta = child;
                                if(alpha >= beta){
                                    killerMove[depth][1] = killerMove[depth][0];
                                    killerMove[depth][0] = hashMove;
                                    goto DONE2;
                                }
                            }
                        }
                        temp = state;
                    }
                    if((cond ^ 7) && !(all & (1u << (pr + 1u)))){
                        temp.red_king_move(i, pr + 1u);
                        int16_t child = dfs<false>(temp, depth + 1, alpha, beta);
                        if(res > child){
                            res = child;
                            hashMove = (((i << 6) | (pr + 1)) << 2) | 1;
                            if(beta > child){
                                beta = child;
                                if(alpha >= beta){
                                    killerMove[depth][1] = killerMove[depth][0];
                                    killerMove[depth][0] = hashMove;
                                    goto DONE2;
                                }
                            }
                        }
                        temp = state;
                    }
                }
                if(i > 3){
                    if(cond && !(all & (1u << (pr - 8u)))){
                        temp.red_king_move(i, pr - 8u);
                        int16_t child = dfs<false>(temp, depth + 1, alpha, beta);
                        if(res > child){
                            res = child;
                            hashMove = (((i << 6) | (pr - 8u)) << 2) | 1;
                            if(beta > child){
                                beta = child;
                                if(alpha >= beta){
                                    killerMove[depth][1] = killerMove[depth][0];
                                    killerMove[depth][0] = hashMove;
                                    goto DONE2;
                                }
                            }
                        }
                        temp = state;
                    }
                    if((cond ^ 7) && !(all & (1u << (pr - 7u)))){
                        temp.red_king_move(i, pr - 7u);
                        int16_t child = dfs<false>(temp, depth + 1, alpha, beta);
                        if(res > child){
                            res = child;
                            hashMove = (((i << 6) | (pr - 7u)) << 2) | 1;
                            if(beta > child){
                                beta = child;
                                if(alpha >= beta){
                                    killerMove[depth][1] = killerMove[depth][0];
                                    killerMove[depth][0] = hashMove;
                                    goto DONE2;
                                }
                            }
                        }
                        temp = state;
                    }
                }
                bits &= (bits - 1u);
            }
        }
        else{
            bits = temp.red_pawn & (attack_left | attack_right);
            while (bits) {
                uint32_t i = __builtin_ctz(bits);
                uint32_t mp = 1u << i;
                if(mp & attack_left){
                    temp.red_pawn_attack(i, i - 9u);
                    int16_t child = dfs_pom<true, false>(temp, depth, i - 9u, alpha, beta);
                    if(res > child){
                        res = child;
                        hashMove = (((i << 6) | (i - 9u)) << 2) | 2;
                        if(beta > child){
                            beta = child;
                            if(alpha >= beta){
                                goto DONE2;
                            }
                        }
                    }
                    temp = state;
                }
                if(mp & attack_right){
                    temp.red_pawn_attack(i, i - 7u);
                    int16_t child = dfs_pom<true, false>(temp, depth, i - 7u, alpha, beta);
                    if(res > child){
                        res = child;
                        hashMove = (((i << 6) | (i - 7u)) << 2) | 2;
                        if(beta > child){
                            beta = child;
                            if(alpha >= beta){
                                goto DONE2;
                            }
                        }
                    }
                    temp = state;
                }
                bits &= (bits - 1u);
            }
            bits = temp.red_king & (attack_left | attack_right | attack_back_left | attack_back_right);
            while (bits) {
                uint32_t i = __builtin_ctz(bits);
                uint32_t mp = 1u << i;
                if(mp & attack_left){
                    temp.red_king_attack(i, i - 9u);
                    int16_t child = dfs_pom<true, true>(temp, depth, i - 9u, alpha, beta);
                    if(res > child){
                        res = child;
                        hashMove = (((i << 6) | (i - 9u)) << 2) | 3;
                        if(beta > child){
                            beta = child;
                            if(alpha >= beta){
                                goto DONE2;
                            }
                        }
                    }
                    temp = state;
                }
                if(mp & attack_right){
                    temp.red_king_attack(i, i - 7u);
                    int16_t child = dfs_pom<true, true>(temp, depth, i - 7u, alpha, beta);
                    if(res > child){
                        res = child;
                        hashMove = (((i << 6) | (i - 7u)) << 2) | 3;
                        if(beta > child){
                            beta = child;
                            if(alpha >= beta){
                                goto DONE2;
                            }
                        }
                    }
                    temp = state;
                }

                if(mp & attack_back_left){
                    temp.red_king_attack(i, i + 7u);
                    int16_t child = dfs_pom<true, true>(temp, depth, i + 7u, alpha, beta);
                    if(res > child){
                        res = child;
                        hashMove = (((i << 6) | (i + 7u)) << 2) | 3;
                        if(beta > child){
                            beta = child;
                            if(alpha >= beta){
                                goto DONE2;
                            }
                        }
                    }
                    temp = state;
                }
                if(mp & attack_back_right){
                    temp.red_king_attack(i, i + 9u);
                    int16_t child = dfs_pom<true, true>(temp, depth, i + 9u, alpha, beta);
                    if(res > child){
                        res = child;
                        hashMove = (((i << 6) | (i + 9u)) << 2) | 3;
                        if(beta > child){
                            beta = child;
                            if(alpha >= beta){
                                goto DONE2;
                            }
                        }
                    }
                    temp = state;
                }

                bits &= (bits - 1u);
            }
        }
    DONE2:
        if(LIKELY(!stop_iterating)){
            uint8_t flag;
            if(res <= origAlpha) flag = FLAG_UPPER;
            else if(res >= origBeta) flag = FLAG_LOWER;
            else flag = FLAG_EXACT;
            tt.insert(state, rem, res, hashMove, flag, Tura);
        }
        return res;
    }
}
void make_move_from_string(Game &state, string move);
#endif
