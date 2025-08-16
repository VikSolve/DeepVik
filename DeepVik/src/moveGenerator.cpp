#include "moveGenerator.h"
#define INLINE __attribute__((always_inline))inline
#define NOINLINE __attribute__((noinline))
#define LIKELY(x)   (__builtin_expect(!!(x), 1))
#define UNLIKELY(x) (__builtin_expect(!!(x), 0))
#define ABS(x) ((x) < 0 ? -(x) : (x))


std::string translate(uint32_t x){
    std::string res = "";
	res += ('A' + ((x & 3) << 1) + ((x >> 2) & 1));
	res += ('1' + (x >> 2));
	return res;
}

std::vector<std::string> cap(Game state, bool tura, uint32_t nr, bool king){
    std::vector<std::string> res;
	Game temp = state;
	bool mv = true;
	uint32_t red = temp.red_pawn | temp.red_king;
	uint32_t black = temp.black_pawn | temp.black_king;
	uint32_t all = black | red;
	uint32_t pm = 1u << nr;
	uint32_t attack_left, attack_right, attack_back_left, attack_back_right;
	if(!tura){
		attack_left = (black & 0x00EEEEEE) & (((red & 0x0F0F0F0F) >> 4) | ((red & 0xF0F0F0F0) >> 3)) & ((~all) >> 7);
		attack_right = (black & 0x00777777) & (((red & 0x0F0F0F0F) >> 5) | ((red & 0xF0F0F0F0) >> 4)) & ((~all) >> 9);
		attack_back_left = (temp.black_king & 0xEEEEEE00) & (((red & 0x0F0F0F0F) << 4) | ((red & 0xF0F0F0F0) << 5)) & (~(all) << 9);
		attack_back_right = (temp.black_king & 0x77777700) & (((red & 0x0F0F0F0F) << 3) | ((red & 0xF0F0F0F0) << 4)) & (~(all) << 7);
		if(!king){
			if(temp.black_pawn & pm){
				if(pm & attack_left){
					temp.black_pawn_attack(nr, nr + 7u);
                    std::vector<std::string> pod = cap(temp, tura, nr + 7, false);
					for(auto v : pod){
						res.push_back(v);
					}
					temp = state;
					mv = false;
				}
				if(pm & attack_right){
					temp.black_pawn_attack(nr, nr + 9u);
                    std::vector<std::string> pod = cap(temp, tura, nr + 9, false);
					for(auto v : pod){
						res.push_back(v);
					}
					temp = state;
					mv = false;
				}
			}
		}
		else{
			if(pm & attack_left){
				temp.black_king_attack(nr, nr + 7u);
                std::vector<std::string> pod = cap(temp, tura, nr + 7u, true);
				for(auto v : pod){
					res.push_back(v);
				}
				temp = state;
				mv = false;
			}
			if(pm & attack_right){
				temp.black_king_attack(nr, nr + 9u);
                std::vector<std::string> pod = cap(temp, tura, nr + 9u, true);
				for(auto v : pod){
					res.push_back(v);
				}
				temp = state;
				mv = false;
			}
			if(pm & attack_back_left){
				temp.black_king_attack(nr, nr - 9u);
                std::vector<std::string> pod = cap(temp, tura, nr - 9, true);
				for(auto v : pod){
					res.push_back(v);
				}
				temp = state;
				mv = false;
			}
			if(pm & attack_back_right){
				temp.black_king_attack(nr, nr - 7u);
                std::vector<std::string> pod = cap(temp, tura, nr - 7, true);
				for(auto v : pod){
					res.push_back(v);
				}
				temp = state;
				mv = false;
			}
		}
		if(mv) res.push_back(translate(nr));
		else{
			for(auto &v : res){
				v = translate(nr) + v;
			}
		}
		return res;
	}
	else{
		attack_left = (red & 0xEEEEEE00) & (((black & 0x0F0F0F0F) << 4) | ((black & 0xF0F0F0F0) << 5)) & (~(all) << 9);
		attack_right = (red & 0x77777700) & (((black & 0x0F0F0F0F) << 3) | ((black & 0xF0F0F0F0) << 4)) & (~(all) << 7);
		attack_back_left = (temp.red_king & 0x00EEEEEE) & (((black & 0x0F0F0F0F) >> 4) | ((black & 0xF0F0F0F0) >> 3)) & ((~all) >> 7);
		attack_back_right = (temp.red_king & 0x00777777) & (((black & 0x0F0F0F0F) >> 5) | ((black & 0xF0F0F0F0) >> 4)) & ((~all) >> 9);
		if(!king){
			if(temp.red_pawn & pm){
				if(pm & attack_left){
					temp.red_pawn_attack(nr, nr - 9u);
                    std::vector<std::string> pod = cap(temp, tura, nr - 9, false);
					for(auto v : pod){
						res.push_back(v);
					}
					temp = state;
					mv = false;
				}
				if(pm & attack_right){
					temp.red_pawn_attack(nr, nr - 7u);
                    std::vector<std::string> pod = cap(temp, tura, nr - 7, false);
					for(auto v : pod){
						res.push_back(v);
					}
					temp = state;
					mv = false;
				}
			}
		}
		else{
			if(pm & attack_left){
				temp.red_king_attack(nr, nr - 9u);
                std::vector<std::string> pod = cap(temp, tura, nr - 9, true);
				for(auto v : pod){
					res.push_back(v);
				}
				temp = state;
				mv = false;
			}
			if(pm & attack_right){
				temp.red_king_attack(nr, nr - 7u);
                std::vector<std::string> pod = cap(temp, tura, nr - 7, true);
				for(auto v : pod){
					res.push_back(v);
				}
				temp = state;
				mv = false;
			}
			if(pm & attack_back_left){
				temp.red_king_attack(nr, nr + 7u);
                std::vector<std::string> pod = cap(temp, tura, nr + 7, true);
				for(auto v : pod){
					res.push_back(v);
				}
				temp = state;
				mv = false;
			}
			if(pm & attack_back_right){
				temp.red_king_attack(nr, nr + 9u);
                std::vector<std::string> pod = cap(temp, tura, nr + 9, true);
				for(auto v : pod){
					res.push_back(v);
				}
				temp = state;
				mv = false;
			}
		}
		if(mv) res.push_back(translate(nr));
		else{
			for(auto &v : res){
				v = translate(nr) + v;
			}
		}
		return res;
	}
}
std::vector<std::string> generateMoves(Game state, bool tura){
    std::vector<std::string> res;
	Game temp = state;
	uint32_t red = temp.red_king | temp.red_pawn;
	uint32_t black = temp.black_king | temp.black_pawn;
	uint32_t all = red | black;
	uint32_t attack_left, attack_right, attack_back_left, attack_back_right;
	if(!tura){
		attack_left = (black & 0x00EEEEEE) & (((red & 0x0F0F0F0F) >> 4) | ((red & 0xF0F0F0F0) >> 3)) & ((~all) >> 7);
		attack_right = (black & 0x00777777) & (((red & 0x0F0F0F0F) >> 5) | ((red & 0xF0F0F0F0) >> 4)) & ((~all) >> 9);
		attack_back_left = (temp.black_king & 0xEEEEEE00) & (((red & 0x0F0F0F0F) << 4) | ((red & 0xF0F0F0F0) << 5)) & (~(all) << 9);
		attack_back_right = (temp.black_king & 0x77777700) & (((red & 0x0F0F0F0F) << 3) | ((red & 0xF0F0F0F0) << 4)) & (~(all) << 7);
		if(attack_left | attack_right | attack_back_left | attack_back_right){
			uint32_t bits = temp.black_pawn & (attack_left | attack_right);
			while (bits) {
				uint32_t i = __builtin_ctz(bits);
				uint32_t mp = 1u << i;
				if(mp & attack_left){
					temp.black_pawn_attack(i, i + 7);
                    std::vector<std::string> pod = cap(temp, tura, i + 7, false);
					for(auto v : pod){
						res.push_back(translate(i) + v);
					}
					temp = state;
				}
				if(mp & attack_right){
					temp.black_pawn_attack(i, i + 9u);
                    std::vector<std::string> pod = cap(temp, tura, i + 9, false);
					for(auto v : pod){
						res.push_back(translate(i) + v);
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
                    std::vector<std::string> pod = cap(temp, tura, i + 7, true);
					for(auto v : pod){
						res.push_back(translate(i) + v);
					}
					temp = state;
				}
				if(mp & attack_right){
					temp.black_king_attack(i, i + 9u);
                    std::vector<std::string> pod = cap(temp, tura, i + 9, true);
					for(auto v : pod){
						res.push_back(translate(i) + v);
					}
					temp = state;
				}
				if(mp & attack_back_left){
					temp.black_king_attack(i, i - 9u);
                    std::vector<std::string> pod = cap(temp, tura, i - 9, true);
					for(auto v : pod){
						res.push_back(translate(i) + v);
					}
					temp = state;
				}
				if(mp & attack_back_right){
					temp.black_king_attack(i, i - 7u);
                    std::vector<std::string> pod = cap(temp, tura, i - 7, true);
                    for(auto v : pod){
						res.push_back(translate(i) + v);
					}
					temp = state;
				}
				bits &= (bits - 1u);
			}
		}
		else{
			uint32_t bits = temp.black_pawn & 0x0FFFFFFF;
			while (bits) {
				uint32_t i = __builtin_ctz(bits);
				uint16_t cond = i & 7u;
				uint32_t pr = i + ((i >> 2u) & 1u) + 3u;
				if(cond && !(all & (1u << pr))){
					res.push_back(translate(i) + translate(pr));
				}
				if((cond ^ 7) && !(all & (1u << (pr + 1)))){
					res.push_back(translate(i) + translate(pr + 1));
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
						res.push_back(translate(i) + translate(pr));
					}
					if((cond ^ 7) && !(all & (1u << (pr + 1)))){
						res.push_back(translate(i) + translate(pr + 1));
					}
				}
				if(i > 3){
					if(cond && !(all & (1u << (pr - 8u)))){
						res.push_back(translate(i) + translate(pr - 8));
						temp = state;
					}
					if((cond ^ 7) && !(all & (1u << (pr - 7u)))){
						res.push_back(translate(i) + translate(pr - 7u));
						temp = state;
					}
				}
				bits &= (bits - 1u);
			}
		}
	}
	else{
		attack_left = (red & 0xEEEEEE00) & (((black & 0x0F0F0F0F) << 4) | ((black & 0xF0F0F0F0) << 5)) & (~(all) << 9);
		attack_right = (red & 0x77777700) & (((black & 0x0F0F0F0F) << 3) | ((black & 0xF0F0F0F0) << 4)) & (~(all) << 7);
		attack_back_left = (temp.red_king & 0x00EEEEEE) & (((black & 0x0F0F0F0F) >> 4) | ((black & 0xF0F0F0F0) >> 3)) & ((~all) >> 7);
		attack_back_right = (temp.red_king & 0x00777777) & (((black & 0x0F0F0F0F) >> 5) | ((black & 0xF0F0F0F0) >> 4)) & ((~all) >> 9);
		if(attack_left | attack_right | attack_back_left | attack_back_right){
			uint32_t bits = temp.red_pawn & (attack_left | attack_right);
			while (bits) {
				uint32_t i = __builtin_ctz(bits);
				uint32_t mp = 1u << i;
				if(mp & attack_left){
					temp.red_pawn_attack(i, i - 9u);
                    std::vector<std::string> pod = cap(temp, tura, i - 9, false);
					for(auto v : pod){
						res.push_back(translate(i) + v);
					}
					temp = state;
				}
				if(mp & attack_right){
					temp.red_pawn_attack(i, i - 7u);
                    std::vector<std::string> pod = cap(temp, tura, i - 7, false);
					for(auto v : pod){
						res.push_back(translate(i) + v);
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
                    std::vector<std::string> pod = cap(temp, tura, i - 9, true);
					for(auto v : pod){
						res.push_back(translate(i) + v);
					}
					temp = state;
				}
				if(mp & attack_right){
					temp.red_king_attack(i, i - 7u);
                    std::vector<std::string> pod = cap(temp, tura, i - 7, true);
					for(auto v : pod){
						res.push_back(translate(i) + v);
					}
					temp = state;
				}
				if(mp & attack_back_left){
					temp.red_king_attack(i, i + 7u);
                    std::vector<std::string> pod = cap(temp, tura, i + 7, true);
					for(auto v : pod){
						res.push_back(translate(i) + v);
					}
					temp = state;
				}
				if(mp & attack_back_right){
					temp.red_king_attack(i, i + 9u);
                    std::vector<std::string> pod = cap(temp, tura, i + 9, true);
					for(auto v : pod){
						res.push_back(translate(i) + v);
					}
					temp = state;
				}
				bits &= (bits - 1u);
			}
		}
		else{
			uint32_t bits = temp.red_pawn & 0xFFFFFFF0;
			while (bits) {
				uint32_t i = __builtin_ctz(bits);
				uint16_t cond = i & 7u;
				uint32_t pr = i + ((i >> 2u) & 1u) - 5u;
				if(cond && !(all & (1u << pr))){
					res.push_back(translate(i) + translate(pr));
				}
				if((cond ^ 7) && !(all & (1u << (pr + 1)))){
					res.push_back(translate(i) + translate(pr + 1));
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
						res.push_back(translate(i) + translate(pr));
					}
					if((cond ^ 7) && !(all & (1u << (pr + 1u)))){
						res.push_back(translate(i) + translate(pr + 1));
					}
				}
				if(i > 3){
					if(cond && !(all & (1u << (pr - 8u)))){
						res.push_back(translate(i) + translate(pr - 8));
					}
					if((cond ^ 7) && !(all & (1u << (pr - 7u)))){
						res.push_back(translate(i) + translate(pr - 7));
					}
				}
				bits &= (bits - 1u);
			}
		}
	}
	return res;	
}

