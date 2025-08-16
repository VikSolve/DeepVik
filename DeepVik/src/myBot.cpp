#include "myBot.h"

FastTT tt;
uint8_t MAX_DEPTH;
std::array<int16_t, 12> buff, saved;
int16_t best_result;
uint64_t t0;
bool stop_iterating;
uint16_t timeCount;
uint16_t killerMove[60][2];

INLINE uint64_t getRTime(){
	timeval t;
	gettimeofday(&t,0);
	return t.tv_sec*1000000UL+t.tv_usec;
}
Game::Game(){
    reset();
}
void Game::reset(){
    red_king = 0;
    black_king = 0;
    black_pawn = 0x00000FFF;
    red_pawn = 0xFFF00000;
    tt.clear();
}

FastTT::FastTT() {
    table.resize(TABLE_SIZE);
}

INLINE Node FastTT::find(uint64_t key) const {
	const Node &n = table[key & SIZE_MASK];
	static constexpr Node EMPTY{}; 
	return (n.key == key) ? n : EMPTY;
}

INLINE void FastTT::insert(uint64_t key, uint8_t depth, int16_t score, uint16_t hashMove, uint8_t flag) {
	Node &n = table[key & SIZE_MASK];
	if (n.key != key || depth >= n.depth) {
		n.key = key;
		n.score = score;
		n.hashMove = hashMove;
		n.depth = depth;
		n.flag = flag;
	}
}

void FastTT::delete_root(const Game &g, bool tura) {
	uint64_t key = g.getHash(tura);
	Node &n = table[key & SIZE_MASK];
	n.depth = 0;
}

void FastTT::clear(){
    table.clear();
    table.resize(TABLE_SIZE);
}


std::string move_recovery(const Game &state, bool my_color){
	int16_t hashMove = tt.find(state.getHash(my_color)).hashMove;
    std::string res = "";
	uint32_t from = hashMove >> 8;
	uint32_t to = (hashMove >> 2) & 31;
    res += ('A' + ((from & 3) << 1) + ((from >> 2) & 1));
	res += ('1' + (from >> 2));
	res += ('A' + ((to & 3) << 1) + ((to >> 2) & 1));
	res += ('1' + (to >> 2));
	for(int i = 1; i < saved[11];i++){
		res += ('A' + ((saved[i] & 3) << 1) + ((saved[i] >> 2) & 1));
		res += ('1' + (saved[i] >> 2));
	}
	return res;
}

std::pair<int, std::string> BestMove(Game &state, int depthLimit, bool my_color){
	timeCount = 0;
    t0 = getRTime();
    std::string answer = "";
    stop_iterating = false;
    int bestEval = 0;
    for(int32_t i = 0; i < 58; i++){
        killerMove[i][0] = killerMove[i + 2][0];
        killerMove[i][1] = killerMove[i + 2][1];
    }
    for(int32_t i = 1; i <= depthLimit; i++){
        saved[11] = 0;
        tt.delete_root(state, my_color);
        buff[11] = 0;
        best_result = (my_color) ? 10001 : -10001;
        MAX_DEPTH = i;
        Game temp = state;
        int tempEval;
        if(!my_color) tempEval = dfs<false>(temp, 0, -10001, 10001);
        else tempEval = dfs<true>(temp, 0, -10001, 10001);
        if(stop_iterating) break;
        //uint64_t ms = (getRTime() - t0)/1000;
        //cerr<<i<<": ms "<<ms<<"\n";
        answer = move_recovery(state, my_color);
        bestEval = tempEval;
    }
    return {bestEval, answer};
}

void update_board(Game &state, std::vector<std::string> &board){
	state.black_pawn = state.red_pawn = state.black_king = state.red_king = 0u;
	uint32_t lic = 0u;
	for(uint32_t i = 0u; i < 8u; i++){
		for(uint32_t j = i & 1u; j < 8u; j += 2u){
			if(board[i][j] == 'b') state.black_pawn ^= (1u << lic);
			else if(board[i][j] == 'B') state.black_king ^= (1u << lic);
			else if(board[i][j] == 'r') state.red_pawn ^= (1u << lic);
			else if(board[i][j] == 'R') state.red_king ^= (1u << lic);
			lic++;
		}	
	}
}

void make_move_from_string(Game &state, std::string move){
    uint32_t move_size = move.size();
    uint32_t curr = (((uint32_t)(move[1] - '1')) << 2u) + (((uint32_t)(move[0] - 'A')) >> 1u);
    for(uint32_t i = 2u; i < move_size; i += 2){
        uint32_t new_pos = (((uint32_t)(move[i + 1u] - '1')) << 2u) + (((uint32_t)(move[i] - 'A')) >> 1u);
        uint32_t sub = (curr > new_pos) ? curr - new_pos : new_pos - curr;
        uint32_t loc = (1u << curr);
        if(sub > 6u){
            if(state.black_pawn & loc) state.black_pawn_attack(curr, new_pos);
            else if(state.red_pawn & loc) state.red_pawn_attack(curr, new_pos);
            else if(state.black_king & loc) state.black_king_attack(curr, new_pos);
            else state.red_king_attack(curr, new_pos);
        }
        else{
            if(state.black_pawn & loc) state.black_pawn_move(curr, new_pos);
            else if(state.red_pawn & loc) state.red_pawn_move(curr, new_pos);
            else if(state.black_king & loc) state.black_king_move(curr, new_pos);
            else state.red_king_move(curr, new_pos);
        }
        curr = new_pos;
    }
}
