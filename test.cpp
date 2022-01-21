#include "board.hpp"
#include <assert.h>
#include <ctime>

const int time_T = 1000000;
int board[time_T][64];
int nxt_x[time_T], nxt_y[time_T];
Board b_test[time_T];

const int p_max = 11;
const int POW[11] = {1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049};
const int pattern_sum[p_max] = {POW[9], POW[10], POW[10], POW[8], POW[8],
                                POW[8], POW[8], POW[7], POW[6], POW[5], POW[4]};
const int pattern[p_max][64] = {{
                            POW[0], POW[1], POW[2],      0,      0,      0,      0,      0,
                            POW[3], POW[4], POW[5],      0,      0,      0,      0,      0,
                            POW[6], POW[7], POW[8],      0,      0,      0,      0,      0,
                                  0,      0,      0,     0,      0,      0,      0,      0,
                                  0,      0,      0,     0,      0,      0,      0,      0,
                                  0,      0,      0,     0,      0,      0,      0,      0,
                                  0,      0,      0,     0,      0,      0,      0,      0,
                                  0,      0,      0,     0,      0,      0,      0,      0
                            },{
                            POW[0], POW[1], POW[2], POW[3], POW[4],      0,      0,      0,
                            POW[5], POW[6], POW[7], POW[8], POW[9],      0,      0,      0,
                                  0,      0,      0,     0,      0,      0,      0,      0,
                                  0,      0,      0,     0,      0,      0,      0,      0,
                                  0,      0,      0,     0,      0,      0,      0,      0,
                                  0,      0,      0,     0,      0,      0,      0,      0,
                                  0,      0,      0,     0,      0,      0,      0,      0,
                                  0,      0,      0,     0,      0,      0,      0,      0
                            },{
                            POW[0], POW[1], POW[2], POW[3], POW[4], POW[5], POW[6], POW[7],
                                 0, POW[8],      0,      0,      0,      0, POW[9],      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0
                            },{
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                            POW[0], POW[1], POW[2], POW[3], POW[4], POW[5], POW[6], POW[7],
                                 0,      0,      0,      0,      0,      0,      0,      0,  
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0   
                            },{
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                            POW[0], POW[1], POW[2], POW[3], POW[4], POW[5], POW[6], POW[7],
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0
                            },{
                                 0,      0,      0,      0,      0,      0,      0,      0,
                            POW[0], POW[1], POW[2], POW[3], POW[4], POW[5], POW[6], POW[7],
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0
                            },{
                                 0,      0,      0,      0,      0,      0,      0, POW[7],
                                 0,      0,      0,      0,      0,      0, POW[6],      0,
                                 0,      0,      0,      0,      0, POW[5],      0,      0,
                                 0,      0,      0,      0, POW[4],      0,      0,      0,
                                 0,      0,      0, POW[3],      0,      0,      0,      0,
                                 0,      0, POW[2],      0,      0,      0,      0,      0,
                                 0, POW[1],      0,      0,      0,      0,      0,      0,
                            POW[0],      0,      0,      0,      0,      0,      0,      0,
                            },{
                                 0,      0,      0,      0,      0,      0, POW[6],      0,
                                 0,      0,      0,      0,      0, POW[5],      0,      0,
                                 0,      0,      0,      0, POW[4],      0,      0,      0,
                                 0,      0,      0, POW[3],      0,      0,      0,      0,
                                 0,      0, POW[2],      0,      0,      0,      0,      0,
                                 0, POW[1],      0,      0,      0,      0,      0,      0,
                            POW[0],      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                            },{
                                 0,      0,      0,      0,      0, POW[5],      0,      0,
                                 0,      0,      0,      0, POW[4],      0,      0,      0,
                                 0,      0,      0, POW[3],      0,      0,      0,      0,
                                 0,      0, POW[2],      0,      0,      0,      0,      0,
                                 0, POW[1],      0,      0,      0,      0,      0,      0,
                            POW[0],      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                            },{
                                 0,      0,      0,      0, POW[4],      0,      0,      0,
                                 0,      0,      0, POW[3],      0,      0,      0,      0,
                                 0,      0, POW[2],      0,      0,      0,      0,      0,
                                 0, POW[1],      0,      0,      0,      0,      0,      0,
                            POW[0],      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                            },{
                                 0,      0,      0, POW[3],      0,      0,      0,      0,
                                 0,      0, POW[2],      0,      0,      0,      0,      0,
                                 0, POW[1],      0,      0,      0,      0,      0,      0,
                            POW[0],      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                                 0,      0,      0,      0,      0,      0,      0,      0,
                            }};

int pos_cnt[64] = {0};
int pos_r[64][p_max<<3];
int pos_p[64][p_max<<3];
int pos_w[64][p_max<<3];

void debug_board(int* board){
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            std::cout << board[i<<3|j] << ' ';
        }std::cout << std::endl;
    }std::cout << std::endl;
}

void debug_move(const u64& move){
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            int ind = (i<<3|j);
            std::cout << (move>>ind&1) << ' ';
        }std::cout << std::endl;
    }std::cout << std::endl;
}

inline int getIndex(int x, int y){return x<<3|y;}

void pattern_precompute(){
    for(int i = 0; i < 64; ++i)pos_cnt[i] = 0;
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            int ind = getIndex(i, j);
            for(int k = 0; k < p_max; ++k){
                int now_k = k;
                int trans = ind;
                if(pattern[k][trans] > 0){
                    pos_r[ind][pos_cnt[ind]] = k;
                    pos_p[ind][pos_cnt[ind]] = now_k;
                    pos_w[ind][pos_cnt[ind]] = pattern[k][trans];
                    pos_cnt[ind]++;
                }
                trans = getIndex(7-i, j);
                now_k += p_max;
                if(pattern[k][trans] > 0){
                    pos_r[ind][pos_cnt[ind]] = k;
                    pos_p[ind][pos_cnt[ind]] = now_k;
                    pos_w[ind][pos_cnt[ind]] = pattern[k][trans];
                    pos_cnt[ind]++;
                }
                trans = getIndex(i, 7-j);
                now_k += p_max;
                if(pattern[k][trans] > 0){
                    pos_r[ind][pos_cnt[ind]] = k;
                    pos_p[ind][pos_cnt[ind]] = now_k;
                    pos_w[ind][pos_cnt[ind]] = pattern[k][trans];
                    pos_cnt[ind]++;
                }
                trans = getIndex(7-i, 7-j);
                now_k += p_max;
                if(pattern[k][trans] > 0){
                    pos_r[ind][pos_cnt[ind]] = k;
                    pos_p[ind][pos_cnt[ind]] = now_k;
                    pos_w[ind][pos_cnt[ind]] = pattern[k][trans];
                    pos_cnt[ind]++;
                }
                trans = getIndex(j, i);
                now_k += p_max;
                if(pattern[k][trans] > 0){
                    pos_r[ind][pos_cnt[ind]] = k;
                    pos_p[ind][pos_cnt[ind]] = now_k;
                    pos_w[ind][pos_cnt[ind]] = pattern[k][trans];
                    pos_cnt[ind]++;
                }
                trans = getIndex(7-j, i);
                now_k += p_max;
                if(pattern[k][trans] > 0){
                    pos_r[ind][pos_cnt[ind]] = k;
                    pos_p[ind][pos_cnt[ind]] = now_k;
                    pos_w[ind][pos_cnt[ind]] = pattern[k][trans];
                    pos_cnt[ind]++;
                }
                trans = getIndex(j, 7-i);
                now_k += p_max;
                if(pattern[k][trans] > 0){
                    pos_r[ind][pos_cnt[ind]] = k;
                    pos_p[ind][pos_cnt[ind]] = now_k;
                    pos_w[ind][pos_cnt[ind]] = pattern[k][trans];
                    pos_cnt[ind]++;
                }
                trans = getIndex(7-j, 7-i);
                now_k += p_max;
                if(pattern[k][trans] > 0){
                    pos_r[ind][pos_cnt[ind]] = k;
                    pos_p[ind][pos_cnt[ind]] = now_k;
                    pos_w[ind][pos_cnt[ind]] = pattern[k][trans];
                    pos_cnt[ind]++;
                }
            }
        }
    }
}

void Board_init_test(){
    int board[8];
    for(int i = 0; i < (1<<16); ++i){
        int t = 0;
        bool valid_flag = true;
        for(int j = 0; j < 8; ++j){
            board[j] = (i>>(j<<1)&0x3);
            if(board[j] == 3){valid_flag = false; break;}
        }
        if(!valid_flag){
            assert(t == Board::pre_move[i]);
            continue;
        }
        for(int j = 0; j < 8; ++j){
            if(board[j] == 2){
                bool flag = false;
                if(j > 1 && board[j-1] == 1){
                    for(int k = j-2; k >= 0; --k){
                        if(board[k] == 2)break;
                        if(board[k] == 0){flag = true; break;}
                    }
                }
                if(flag){t |= (1<<j); continue;}
                if(j < 6 && board[j+1] == 1){
                    for(int k = j+2; k < 8; ++k){
                        if(board[k] == 2)break;
                        if(board[k] == 0){flag = true; break;}
                    }
                }
                if(flag){t |= (1<<j); continue;}
            }
        }
        assert(t == Board::pre_move[i]);
    }
}

void Board_encode_test(){
    int board[64], board_test[64];
    for(int i = 0; i < 64; ++i)board[i] = rand()%3;
    Board b_test;
    b_test.encode(board);
    b_test.decode(board_test);
    for(int i = 0; i < 64; ++i)assert(board[i] == board_test[i]);
}

void Board_decode_test(){
    Board b_test;
    b_test.b[0] = rand()*rand()*rand()*rand();
    b_test.b[1] = rand()*rand()*rand()*rand();
    int board[64];
    b_test.decode(board);
    u64 b0 = b_test.b[0], b1 = b_test.b[1];
    b_test.encode(board);
    assert(b_test.b[0] == b0);
    assert(b_test.b[1] == b1);
}

void Board_calNum_test(){
    int board[64];
    for(int i = 0; i < 64; ++i)board[i] = 1;
    Board b_test;
    b_test.encode(board);
    int num[2];
    b_test.calNum(num);
    assert(num[0] == 0);
    assert(num[1] == 64);

    for(int i = 0; i < 64; ++i)board[i] = 0;
    b_test.encode(board);
    b_test.calNum(num);
    assert(num[0] == 64);
    assert(num[1] == 0);

    for(int i = 0; i < 64; ++i)board[i] = rand()%3;
    b_test.encode(board);
    b_test.calNum(num);
    for(int i = 0; i < 64; ++i){
        if(board[i] == 1)num[1]--;
        else if(board[i] == 0)num[0]--;
    }
    assert(num[0] == 0);
    assert(num[1] == 0);
}

void Board_reverse_test(){
    int T = 100;
    while(T--){
        int board[64];
        for(int i = 0; i < 64; ++i)board[i] = rand()%3;
        Board b_test;
        b_test.encode(board);
        b_test.reverse();
        int board_test[64];
        b_test.decode(board_test);
        for(int i = 0; i < 64; ++i){
            if(board[i] == 1)assert(board_test[i] == 0);
            else if(board[i] == 0)assert(board_test[i] == 1);
            else assert(board_test[i] == board[i]);
        }
    }
}

void Board_transpose_test(){
    int T = 100;
    Board b_base, b_trans, b_test;
    while(T--){
        int board[64];
        for(int i = 0; i < 64; ++i)board[i] = rand()%3;
        b_base.encode(board);
        b_base.transpose(&b_test);
        b_base.transpose();
        for(int i = 0; i < 8; ++i){
            for(int j = 0; j < i; ++j){
                std::swap(board[i<<3|j], board[j<<3|i]);
            }
        }
        b_trans.encode(board);
        assert(b_trans == b_test);
        assert(b_trans == b_base);

        u64 move = 0;
        for(int i = 0; i < 64; ++i){
            board[i] = rand()%2;
            move |= ((u64)(board[i]) << i);
        }
        b_base.transpose(move);
        for(int i = 0; i < 8; ++i){
            for(int j = 0; j < i; ++j){
                std::swap(board[i<<3|j], board[j<<3|i]);
            }
        }
        u64 move_test = 0;
        for(int i = 0; i < 64; ++i)move_test |= ((u64)(board[i]) << i);
        assert(move == move_test);
    }
}

void Board_reverseHorizon_test(){
    int T = 100;
    Board b_base, b_trans, b_test;
    while(T--){
        int board[64];
        for(int i = 0; i < 64; ++i)board[i] = rand()%3;
        b_base.encode(board);
        b_base.reverseHorizon(&b_test);
        b_base.reverseHorizon();
        for(int i = 0; i < 8; ++i){
            for(int j = 0; j < 4; ++j){
                std::swap(board[i<<3|j], board[i<<3|(7-j)]);
            }
        }
        b_trans.encode(board);
        assert(b_trans == b_test);
        assert(b_trans == b_base);

        u64 move = 0;
        for(int i = 0; i < 64; ++i){
            board[i] = rand()%2;
            move |= ((u64)(board[i]) << i);
        }
        b_base.reverseHorizon(move);
        for(int i = 0; i < 8; ++i){
            for(int j = 0; j < 4; ++j){
                std::swap(board[i<<3|j], board[i<<3|(7-j)]);
            }
        }
        u64 move_test = 0;
        for(int i = 0; i < 64; ++i)move_test |= ((u64)(board[i]) << i);
        assert(move == move_test);
    }
}

void Board_reverseVerticle_test(){
    int T = 100;
    Board b_base, b_trans, b_test;
    while(T--){
        int board[64];
        for(int i = 0; i < 64; ++i)board[i] = rand()%3;
        b_base.encode(board);
        b_base.reverseVerticle(&b_test);
        b_base.reverseVerticle();
        for(int i = 0; i < 4; ++i){
            for(int j = 0; j < 8; ++j){
                std::swap(board[i<<3|j], board[(7-i)<<3|j]);
            }
        }
        b_trans.encode(board);
        assert(b_trans == b_test);
        assert(b_trans == b_base);

        u64 move = 0;
        for(int i = 0; i < 64; ++i){
            board[i] = rand()%2;
            move |= ((u64)(board[i]) << i);
        }
        b_base.reverseHorizon(move);
        for(int i = 0; i < 8; ++i){
            for(int j = 0; j < 4; ++j){
                std::swap(board[i<<3|j], board[i<<3|(7-j)]);
            }
        }
        u64 move_test = 0;
        for(int i = 0; i < 64; ++i)move_test |= ((u64)(board[i]) << i);
        assert(move == move_test);
    }
}

void Board_diagCompress_test(){
    int T = 100;
    Board b_base, b_trans, b_test;
    while(T--){
        int board[64], board_tmp[64];
        for(int i = 0; i < 64; ++i)board[i] = rand()%3;
        b_base.encode(board);
        b_base.diagCompress(&b_test);
        b_base.diagCompress();
        //board(i, j) -> board(i+j, j) if i+j < 8
        //board(i, j) -> board(i+j-8, j) if i+j >= 8
        for(int i = 0; i < 8; ++i){
            for(int j = 0; j < 8; ++j){
                if(i+j < 8)board_tmp[(i+j)<<3|j] = board[i<<3|j];
                else board_tmp[(i+j-8)<<3|j] = board[i<<3|j];
            }
        }
        b_trans.encode(board_tmp);
        assert(b_trans == b_test);
        assert(b_trans == b_base);

        u64 move = 0;
        for(int i = 0; i < 64; ++i){
            board[i] = rand()%2;
            move |= ((u64)(board[i]) << i);
        }
        b_base.diagCompress(move);
        for(int i = 0; i < 8; ++i){
            for(int j = 0; j < 8; ++j){
                if(i+j < 8)board_tmp[(i+j)<<3|j] = board[i<<3|j];
                else board_tmp[(i+j-8)<<3|j] = board[i<<3|j];
            }
        }
        u64 move_test = 0;
        for(int i = 0; i < 64; ++i)move_test |= ((u64)(board_tmp[i]) << i);
        assert(move == move_test);
    }
}

void Board_diagCompressInverse_test(){
    int T = 100;
    Board b_base, b_trans, b_test;
    while(T--){
        int board[64], board_tmp[64];
        for(int i = 0; i < 64; ++i)board[i] = rand()%3;
        b_base.encode(board);
        b_base.diagCompressInverse(&b_test);
        b_base.diagCompressInverse();

        for(int i = 0; i < 8; ++i){
            for(int j = 0; j < 8; ++j){
                if(i+j < 8)board_tmp[i<<3|j] = board[(i+j)<<3|j];
                else board_tmp[i<<3|j] = board[(i+j-8)<<3|j];
            }
        }
        b_trans.encode(board_tmp);

        assert(b_trans == b_test);
        assert(b_trans == b_base);

        b_test.encode(board_tmp);
        b_test.diagCompress();
        b_test.diagCompressInverse(&b_base);
        b_test.diagCompressInverse();
        assert(b_trans == b_test);
        assert(b_trans == b_base);

        u64 move = 0;
        for(int i = 0; i < 64; ++i){
            board[i] = rand()%2;
            move |= ((u64)(board[i]) << i);
        }
        b_base.diagCompressInverse(move);
        for(int i = 0; i < 8; ++i){
            for(int j = 0; j < 8; ++j){
                if(i+j < 8)board_tmp[i<<3|j] = board[(i+j)<<3|j];
                else board_tmp[i<<3|j] = board[(i+j-8)<<3|j];
            }
        }
        u64 move_test = 0;
        for(int i = 0; i < 64; ++i)move_test |= ((u64)(board_tmp[i]) << i);
        assert(move == move_test);

        b_base.diagCompress(move_test);
        b_base.diagCompressInverse(move_test);
        assert(move == move_test);
    }
}

void Board_checkHorizon_test(){
    int T = 100;
    Board b_test;
    while(T--){
        int board[64];
        for(int i = 0; i < 64; ++i)board[i] = rand()%3;
        b_test.encode(board);
        u64 move, move_test;
        move = 0;
        for(int i = 0; i < 8; ++i){
            for(int j = 0; j < 8; ++j){
                int ind = (i<<3|j);
                if(board[ind] == 2){
                    bool flag = false;
                    if(j > 1 && board[ind-1] == 1){
                        for(int k = j-2; k >= 0; --k){
                            if(board[i<<3|k] == 2)break;
                            if(board[i<<3|k] == 0){flag = true; break;}
                        }
                    }
                    if(j < 6 && board[ind+1] == 1){
                        for(int k = j+2; k < 8; ++k){
                            if(board[i<<3|k] == 2)break;
                            if(board[i<<3|k] == 0){flag = true; break;}
                        }
                    }
                    if(flag)move |= ((u64)(1) << ind);
                }
            }
        }
        b_test.checkHorizon(move_test);
        assert(move == move_test);
    }
}

void Board_checkVerticle_test(){
    int T = 100;
    Board b_test;
    while(T--){
        int board[64];
        for(int i = 0; i < 64; ++i)board[i] = rand()%3;
        b_test.encode(board);
        u64 move, move_test;
        move = 0;
        for(int i = 0; i < 8; ++i){
            for(int j = 0; j < 8; ++j){
                int ind = (i<<3|j);
                if(board[ind] == 2){
                    bool flag = false;
                    if(i > 1 && board[ind-8] == 1){
                        for(int k = i-2; k >= 0; --k){
                            if(board[k<<3|j] == 2)break;
                            if(board[k<<3|j] == 0){flag = true; break;}
                        }
                    }
                    if(i < 6 && board[ind+8] == 1){
                        for(int k = i+2; k < 8; ++k){
                            if(board[k<<3|j] == 2)break;
                            if(board[k<<3|j] == 0){flag = true; break;}
                        }
                    }
                    if(flag)move |= ((u64)(1) << ind);
                }
            }
        }
        b_test.checkVerticle(move_test);
        assert(move == move_test);
    }
}

void Board_checkDiag_test(){
    int T = 100;
    Board b_test;
    while(T--){
        int board[64];
        for(int i = 0; i < 64; ++i)board[i] = rand()%3;
        b_test.encode(board);
        u64 move, move_test;
        move = 0;
        for(int i = 0; i < 8; ++i){
            for(int j = 0; j < 8; ++j){
                int ind = (i<<3|j);
                if(board[ind] == 2){
                    bool flag = false;
                    if(i > 1 && j < 6 && board[(i-1)<<3|(j+1)] == 1){
                        int x = i-2, y = j+2;
                        while(x >= 0 && x < 8 && y >= 0 && y < 8){
                            if(board[x<<3|y] == 2)break;
                            if(board[x<<3|y] == 0){flag = true; break;}
                            --x; ++y;
                        }
                    }
                    if(i < 6 && j > 1 && board[(i+1)<<3|(j-1)] == 1){
                        int x = i+2, y = j-2;
                        while(x >= 0 && x < 8 && y >= 0 && y < 8){
                            if(board[x<<3|y] == 2)break;
                            if(board[x<<3|y] == 0){flag = true; break;}
                            ++x; --y;
                        }
                    }
                    if(flag)move |= ((u64)(1) << ind);
                }
            }
        }
        b_test.checkDiag(move_test);
        assert(move == move_test);
    }
}

void Board_testMove_test(){
    int dx[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dy[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int T = 100;
    Board b_test;
    while(T--){
        int board[64], board_tmp[64];
        for(int i = 0; i < 64; ++i)board[i] = rand()%2;
        b_test.encode(board);
        u64 move, move_test;
        move = 0;
        for(int i = 0; i < 8; ++i){
            for(int j = 0; j < 8; ++j){
                if(board[i<<3|j] != 2)continue;
                bool flag = false;
                for(int k = 0; k < 8; ++k){
                    int x = i+dx[k], y = j+dy[k];
                    if(x >= 0 && y >= 0 && x < 8 && y < 8 && board[x<<3|y] == 1){
                        x = x+dx[k]; y = y+dy[k];
                        while(x >= 0 && y >= 0 && x < 8 && y < 8){
                            if(board[x<<3|y] == 2)break;
                            if(board[x<<3|y] == 0){flag = true; break;}
                            x = x+dx[k]; y = y+dy[k];
                        }
                        if(flag)break;
                    }
                }
                if(flag)move |= ((u64)(1) << (i<<3|j));
            }
        }
        b_test.testMove(move_test);
        assert(move_test == move);
        b_test.decode(board_tmp);
        for(int i = 0; i < 64; ++i)assert(board_tmp[i] == board[i]);
    }
}

void Board_testMove_test_time(){
    int dx[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dy[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    for(int i = 0; i < time_T; ++i){
        for(int j = 0; j < 64; ++j)board[i][j] = rand()%3;
    }
    for(int i = 0; i < time_T; ++i)b_test[i].encode(board[i]);
    u64 move;
    int time_st = clock();
    for(int i = 0; i < time_T; ++i)b_test[i].testMove(move);
    int time_ed = clock();
    std::cout << "testMove binary time = " << time_ed-time_st << std::endl;
    time_st = clock();
    int x, y;
    for(int u = 0; u < time_T; ++u){
        move = 0;
        for(int i = 0; i < 8; ++i){
            for(int j = 0; j < 8; ++j){
                if(board[u][i<<3|j] != 2)continue;
                bool flag = false;
                for(int k = 0; k < 8; ++k){
                    x = i+dx[k], y = j+dy[k];
                    if(x >= 0 && y >= 0 && x < 8 && y < 8 && board[u][x<<3|y] == 1){
                        x = x+dx[k]; y = y+dy[k];
                        while(x >= 0 && y >= 0 && x < 8 && y < 8){
                            if(board[u][x<<3|y] == 2)break;
                            if(board[u][x<<3|y] == 0){flag = true; break;}
                            x = x+dx[k]; y = y+dy[k];
                        }
                        if(flag)break;
                    }
                }
                if(flag)move |= ((u64)(1) << (i<<3|j));
            }
        }
    }
    time_ed = clock();
    std::cout << "testMove naive time = " << time_ed-time_st << std::endl;
}

void Board_nextBoard_test(){
    int dx[] = {0, 0, -1, 1, 1, -1, 1, -1};
    int dy[] = {-1, 1, 0, 0, -1, 1, 1, -1};
    int T = 1000000;
    int board[64], board_test[64];
    Board b_test, b_next, b_check;
    u64 move;
    while(T--){
        for(int i = 0; i < 64; ++i)board[i] = rand()%3;
        b_test.encode(board);
        b_test.testMove(move);
        for(int t = 0; t < 64; ++t){
            if(move >> t & 1){
                for(int i = 0; i < 64; ++i)board_test[i] = board[i];
                int x = (t>>3), y = (t&7);
                for(int k = 0; k < 8; ++k){
                    int i = x+dx[k];
                    int j = y+dy[k];
                    if(i >= 0 && i < 8 && j >= 0 && j < 8 && board_test[i<<3|j] == 1){
                        i = i+dx[k];
                        j = j+dy[k];
                        while(i >= 0 && i < 8 && j >= 0 && j < 8){
                            if(board_test[i<<3|j] == 2)break;
                            if(board_test[i<<3|j] == 0){
                                i = i-dx[k];
                                j = j-dy[k];
                                while(i != x || j != y){
                                    board_test[i<<3|j] = 0;
                                    i = i-dx[k];
                                    j = j-dy[k];
                                }
                                break;
                            }
                            i = i+dx[k];
                            j = j+dy[k];
                        }
                    }
                }
                board_test[t] = 0;
                b_test.nextBoard(&b_next, x, y);
                b_check.encode(board_test);
                assert(b_next == b_check);
            }
        }
    }
}

void Board_nextBoard_test_time(){
    int dx[] = {0, 0, -1, 1, 1, -1, 1, -1};
    int dy[] = {-1, 1, 0, 0, -1, 1, 1, -1};
    for(int i = 0; i < time_T; ++i){
        for(int j = 0; j < 64; ++j)board[i][j] = rand()%3;
    }
    for(int i = 0; i < time_T; ++i)b_test[i].encode(board[i]);
    u64 move;
    for(int i = 0; i < time_T; ++i){
        b_test[i].testMove(move);
        if(move == 0){
            nxt_x[i] = -1;
            nxt_y[i] = -1;
            continue;
        }
        nxt_x[i] = rand()%8; nxt_y[i] = rand()%8;
        while(!((move >> (nxt_x[i]<<3|nxt_y[i])) &1)){
            nxt_x[i] = rand()%8;
            nxt_y[i] = rand()%8;
        }
    }
    int time_st = clock();
    for(int i = 0; i < time_T; ++i)b_test[i].nextBoard(nxt_x[i], nxt_y[i]);
    int time_ed = clock();
    std::cout << "binary time = " << time_ed-time_st << std::endl;
    time_st = clock();
    for(int u = 0; u < time_T; ++u){
        int x = nxt_x[u], y = nxt_y[u];
        for(int k = 0; k < 8; ++k){
            int i = x+dx[k];
            int j = y+dy[k];
            if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[u][i<<3|j] == 1){
                i = i+dx[k];
                j = j+dy[k];
                while(i >= 0 && i < 8 && j >= 0 && j < 8){
                    if(board[u][i<<3|j] == 2)break;
                    if(board[u][i<<3|j] == 0){
                        i = i-dx[k];
                        j = j-dy[k];
                        while(i != x || j != y){
                            board[u][i<<3|j] = 0;
                            i = i-dx[k];
                            j = j-dy[k];
                        }
                        break;
                    }
                    i = i+dx[k];
                    j = j+dy[k];
                }
            }
        }
        board[u][x<<3|y] = 0;
    }
    time_ed = clock();
    std::cout << "naive time = " << time_ed-time_st << std::endl;
}

void Board_calPattern_test(){
    int T = 100;
    int board[64], board_test[64];
    int p_sum[p_max<<3], p_sum_test[p_max<<3];
    Board b_test;
    while(T--){
        for(int i = 0; i < 64; ++i)board[i] = rand()%3;
        for(int i = 0; i < (p_max<<3); ++i)p_sum[i] = 0;
        for(int i = 0; i < 64; ++i){
            for(int j = 0; j < pos_cnt[i]; ++j){
                p_sum[pos_p[i][j]] += board[i]*pos_w[i][j];
            }
        }
        b_test.encode(board);
        b_test.calPattern88(p_sum_test);
        for(int i = 0; i < (p_max<<3); ++i)assert(p_sum[i] == p_sum_test[i]);
        b_test.decode(board_test);
        for(int i = 0; i < 64; ++i)assert(board_test[i] == board[i]);
    }
}

void Board_calPattern_test_time(){
    for(int u = 0; u < time_T; ++u){
        for(int j = 0; j < 64; ++j)board[u][j] = rand()%3;
    }
    for(int i = 0; i < time_T; ++i)b_test[i].encode(board[i]);
    int p_sum[p_max<<3];
    int time_st = clock();
    for(int u = 0; u < time_T; ++u)b_test[u].calPattern88(p_sum);
    int time_ed = clock();
    std::cout << "binary time = " << time_ed-time_st << std::endl;
    time_st = clock();
    for(int u = 0; u < time_T; ++u){
        for(int i = 0; i < (p_max<<3); ++i)p_sum[i] = 0;
        for(int i = 0; i < 64; ++i){
            for(int j = 0; j < pos_cnt[i]; ++j){
                p_sum[pos_p[i][j]] += board[u][i]*pos_w[i][j];
            }
        }
    }
    time_ed = clock();
    std::cout << "naive time = " << time_ed-time_st << std::endl;
}

void Board_splitMove_test(){
    int T = 100;
    u64 move;
    int board[64];
    int move_ind[64], move_cnt = 0;
    int move_ind_test[64], move_cnt_test = 0;
    Board b_test;
    while(T--){
        for(int i = 0; i < 64; ++i)board[i] = rand()%2;
        for(int i = 0; i < 64; ++i)move |= ((u64)(board[i]) << i);
        move_cnt = 0;
        for(int i = 0; i < 64; ++i){
            if(move >> i & 1)move_ind[move_cnt++] = i;
        }
        b_test.splitMove(move_ind_test, move_cnt_test, move);
        assert(move_cnt == move_cnt_test);
        for(int i = 0; i < move_cnt; ++i)assert(move_ind[i] == move_ind_test[i]);
    }
}

int main(){
    srand(998244353);
    Board::init();
    pattern_precompute();

    /*Board_init_test();
    Board_encode_test();
    Board_decode_test();
    Board_calNum_test();
    Board_reverse_test();
    Board_transpose_test();
    Board_reverseHorizon_test();
    Board_reverseVerticle_test();
    Board_diagCompress_test();
    Board_diagCompressInverse_test();
    Board_checkHorizon_test();
    Board_checkVerticle_test();
    Board_checkDiag_test();*/
    Board_testMove_test();
    /*Board_testMove_test_time();
    Board_nextBoard_test();
    Board_nextBoard_test_time();
    Board_calPattern_test();
    Board_calPattern_test_time();
    Board_splitMove_test();*/
}