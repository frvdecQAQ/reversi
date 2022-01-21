#ifndef BOARD_HPP_
#define BOARD_HPP_

#include <cstring>
#include <cstdio>
#include <iostream>
#include <algorithm>

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;

//Board data structure
//for every pos, 2->empty, 0->offensive, 1->defensive, use 2-bit to express

class Board{
public:
    u64 b[2];

    static u64 pre_move[1<<16];
    static int move_pos[1<<8][8];
    static int move_pos_cnt[1<<8];
    static u64 nxt_move[1<<16][8];
    static int base3[1<<20];
    static const int p_max = 11;

    static void init(){
        bool valid_flag;
        //calculate pre_move
        for(u16 i = 0; ; ++i){
            pre_move[i] = 0;
            valid_flag = true;
            u16 r_i = ((u16)~(i | 0x5555))>>1^i;
            for(int j = 0; j < 8; ++j){
                if((r_i>>(j<<1)&0x3) == 3){
                    valid_flag = false;
                    break;
                }
            }
            if(!valid_flag){
                if(i == (1<<16)-1)break;
                continue;
            }
            for(int j = 0; j < 8; ++j){
                if((r_i>>(j<<1)&0x3) == 2){
                    int t = lowbit(r_i>>(j<<1)>>2);
                    if(t & 0x5554)pre_move[i] |= (1<<j);
                }
            }
            r_i = ((r_i & 0xCCCC) >> 2) | ((r_i & 0x3333) << 2);
            r_i = ((r_i & 0xF0F0) >> 4) | ((r_i & 0x0F0F) << 4);
            r_i = (r_i >> 8) | ((r_i & 0xFF) << 8);
            for(int j = 0; j < 8; ++j){
                if((r_i>>(j<<1)&0x3) == 2){
                    int t = lowbit(r_i>>(j<<1)>>2);
                    if(t & 0x5554)pre_move[i] |= (1<<(7-j));
                }
            }
        }
        //calculate nxt_move
        int board_tmp[8];
        for(u16 i = 0; ; ++i){
            valid_flag = true;
            for(int j = 0; j < 8; ++j){
                board_tmp[j] = ((i>>(j<<1))&3);
                if(board_tmp[j] == 3){valid_flag = false; break;}
            }
            if(!valid_flag){
                if(i == (1<<16)-1)break;
                continue;
            }
            for(int j = 0; j < 8; ++j){
                if(pre_move[i] >> j & 1){
                    int r = j+1;
                    while(r < 8 && board_tmp[r] == 1)++r;
                    if(r != 8 && board_tmp[r] == 0){
                        for(int k = j+1; k < r; ++k)board_tmp[k] = 0;
                    }
                    int l = j-1;
                    while(l >= 0 && board_tmp[l] == 1)--l;
                    if(l != -1 && board_tmp[l] == 0){
                        for(int k = j-1; k > l; --k)board_tmp[k] = 0;
                    }
                    for(int k = 0; k < 8; ++k){
                        nxt_move[i][j] |= (board_tmp[k]<<(k<<1));
                        board_tmp[k] = ((i>>(k<<1))&3);
                    }
                }
                else nxt_move[i][j] = i;
            }
        }
        //calculate base3
        for(int i = 0; i < (1<<20); ++i){
            int base = 1;
            base3[i] = 0;
            for(int j = 0; j < 10; ++j){
                base3[i] += ((i>>(j<<1))&3)*base;
                base *= 3;
            }
        }    
        //calculate move_pos
        for(int i = 0; i < (1<<8); ++i){
            move_pos_cnt[i] = 0;
            for(int j = 0; j < 8; ++j){
                if(i>>j&1)move_pos[i][move_pos_cnt[i]++] = j;
            }
        }
    }

    bool operator ==(const Board& t)const{
        return (t.b[0] == b[0] && t.b[1] == b[1]);
    }
    bool operator !=(const Board& t)const{
        return t.b[0] != b[0] || t.b[1] != b[1];
    }

    void encode(int* board){
        //encode board state to 2 64-bit numbers
        //board: input, board state[0, 64)
        b[0] = b[1] = 0;
        for(int i = 0; i < 32; ++i){
            b[0] |= (u64(board[i])<<(i<<1));
            b[1] |= (u64(board[i|0x20])<<(i<<1));
        }
    }
    void decode(int* board){
        //decode 2 64-bit numbers to board state
        //board: output, board state[0, 64)
        for(int i = 0; i < 32; ++i){
            board[i] = (b[0]>>(i<<1)&0x3);
            board[i|0x20] = (b[1]>>(i<<1)&0x3);
        }
    }
    void calNum(int* num){
        //calculate chess num
        //num[0]->chess 0 numbers & num[1]->chess 1 numbers
        //num: output, chess number[0, 2]
        num[0] = 64-popCount(b[0])-popCount(b[1]);
        num[1] = popCount(b[0] & 0x5555555555555555)+popCount(b[1] & 0x5555555555555555);
    }
    void reverse(){
        //reverse 0->1, 1->0
        b[0] = ((u64)(~(b[0] | 0x5555555555555555))>>1)^b[0];
        b[1] = ((u64)(~(b[1] | 0x5555555555555555))>>1)^b[1];
    }
    
    void transpose(){
        //transpose board, board(i, j) -> board(j, i)
        b[0] = ((b[0] & 0x0000CCCC0000CCCC)<<14) | ((b[0] & 0x3333000033330000)>>14) | 
                   (b[0] & 0xCCCC3333CCCC3333);
        b[1] = ((b[1] & 0x0000CCCC0000CCCC)<<14) | ((b[1] & 0x3333000033330000)>>14) | 
                   (b[1] & 0xCCCC3333CCCC3333);
        b[0] = ((b[0] & 0x00000000F0F0F0F0)<<28) | ((b[0] & 0x0F0F0F0F00000000)>>28) |
                   (b[0] & 0xF0F0F0F00F0F0F0F);
        b[1] = ((b[1] & 0x00000000F0F0F0F0)<<28) | ((b[1] & 0x0F0F0F0F00000000)>>28) |
                   (b[1] & 0xF0F0F0F00F0F0F0F);
        u64 transpose_tmp = ((b[0] & 0xFF00FF00FF00FF00)>>8);
        b[0] = (b[0] & 0x00FF00FF00FF00FF) | ((b[1] & 0x00FF00FF00FF00FF)<<8);
        b[1] = (b[1] & 0xFF00FF00FF00FF00) | transpose_tmp;
    }
    void transpose(Board* t){
        //transpose board, board(i, j) -> board(j, i)
        //t: output, transpose of now board
        t->b[0] = ((b[0] & 0x0000CCCC0000CCCC)<<14) | ((b[0] & 0x3333000033330000)>>14) | 
                   (b[0] & 0xCCCC3333CCCC3333);
        t->b[1] = ((b[1] & 0x0000CCCC0000CCCC)<<14) | ((b[1] & 0x3333000033330000)>>14) | 
                   (b[1] & 0xCCCC3333CCCC3333);
        t->b[0] = ((t->b[0] & 0x00000000F0F0F0F0)<<28) | ((t->b[0] & 0x0F0F0F0F00000000)>>28) |
                   (t->b[0] & 0xF0F0F0F00F0F0F0F);
        t->b[1] = ((t->b[1] & 0x00000000F0F0F0F0)<<28) | ((t->b[1] & 0x0F0F0F0F00000000)>>28) |
                   (t->b[1] & 0xF0F0F0F00F0F0F0F);
        u64 transpose_tmp  = ((t->b[0] & 0xFF00FF00FF00FF00)>>8);
        t->b[0] = (t->b[0] & 0x00FF00FF00FF00FF) | ((t->b[1] & 0x00FF00FF00FF00FF)<<8);
        t->b[1] = (t->b[1] & 0xFF00FF00FF00FF00) | transpose_tmp;
    }
    void transpose(u64& move){
        //transpose 8*8 move
        //move:input & output 
        move = ((move & 0x00AA00AA00AA00AA) <<  7) | ((move & 0x5500550055005500) >>  7) |
                (move & 0xAA55AA55AA55AA55);
        move = ((move & 0x0000CCCC0000CCCC) << 14) | ((move & 0x3333000033330000) >> 14) |
                (move & 0xCCCC3333CCCC3333);
        move = ((move & 0x00000000F0F0F0F0) << 28) | ((move & 0x0F0F0F0F00000000) >> 28) |
                (move & 0xF0F0F0F00F0F0F0F);
    }
    
    void reverseHorizon(){
        //reverse board horizontally , board(i, j) -> board(i, 7-j)
        b[0] = ((b[0] & 0xCCCCCCCCCCCCCCCC) >> 2) | ((b[0] & 0x3333333333333333) << 2);
        b[1] = ((b[1] & 0xCCCCCCCCCCCCCCCC) >> 2) | ((b[1] & 0x3333333333333333) << 2);
        b[0] = ((b[0] & 0xF0F0F0F0F0F0F0F0) >> 4) | ((b[0] & 0x0F0F0F0F0F0F0F0F) << 4);
        b[1] = ((b[1] & 0xF0F0F0F0F0F0F0F0) >> 4) | ((b[1] & 0x0F0F0F0F0F0F0F0F) << 4);
        b[0] = ((b[0] & 0xFF00FF00FF00FF00) >> 8) | ((b[0] & 0x00FF00FF00FF00FF) << 8);
        b[1] = ((b[1] & 0xFF00FF00FF00FF00) >> 8) | ((b[1] & 0x00FF00FF00FF00FF) << 8);
    }
    void reverseHorizon(Board* t){
        //reverse board horizontally , board(i, j) -> board(i, 7-j)
        //t: output, reverse result
        t->b[0] = ((b[0] & 0xCCCCCCCCCCCCCCCC) >> 2) | ((b[0] & 0x3333333333333333) << 2);
        t->b[1] = ((b[1] & 0xCCCCCCCCCCCCCCCC) >> 2) | ((b[1] & 0x3333333333333333) << 2);
        t->b[0] = ((t->b[0] & 0xF0F0F0F0F0F0F0F0) >> 4) | ((t->b[0] & 0x0F0F0F0F0F0F0F0F) << 4);
        t->b[1] = ((t->b[1] & 0xF0F0F0F0F0F0F0F0) >> 4) | ((t->b[1] & 0x0F0F0F0F0F0F0F0F) << 4);
        t->b[0] = ((t->b[0] & 0xFF00FF00FF00FF00) >> 8) | ((t->b[0] & 0x00FF00FF00FF00FF) << 8);
        t->b[1] = ((t->b[1] & 0xFF00FF00FF00FF00) >> 8) | ((t->b[1] & 0x00FF00FF00FF00FF) << 8);
    }
    void reverseHorizon(u64& move){
        move = ((move & 0xAAAAAAAAAAAAAAAA) >> 1) | ((move & 0x5555555555555555) << 1);
        move = ((move & 0xCCCCCCCCCCCCCCCC) >> 2) | ((move & 0x3333333333333333) << 2);
        move = ((move & 0xF0F0F0F0F0F0F0F0) >> 4) | ((move & 0x0F0F0F0F0F0F0F0F) << 4);
    }

    void reverseVerticle(){
        //reverse board vertically, board(i, j) -> board(7-i, j)
        b[0] = ((b[0] & 0xFFFF0000FFFF0000) >> 16) | ((b[0] & 0x0000FFFF0000FFFF) << 16);
        b[1] = ((b[1] & 0xFFFF0000FFFF0000) >> 16) | ((b[1] & 0x0000FFFF0000FFFF) << 16);
        b[0] = ((b[0] & 0xFFFFFFFF00000000) >> 32) | ((b[0] & 0x00000000FFFFFFFF) << 32);
        b[1] = ((b[1] & 0xFFFFFFFF00000000) >> 32) | ((b[1] & 0x00000000FFFFFFFF) << 32);
        std::swap(b[0], b[1]);
    }
    void reverseVerticle(Board* t){
        //reverse board vertically, board(i, j) -> board(7-i, j)
        //t: output, reverse result
        t->b[0] = ((b[1] & 0xFFFF0000FFFF0000) >> 16) | ((b[1] & 0x0000FFFF0000FFFF) << 16);
        t->b[1] = ((b[0] & 0xFFFF0000FFFF0000) >> 16) | ((b[0] & 0x0000FFFF0000FFFF) << 16);
        t->b[0] = ((t->b[0] & 0xFFFFFFFF00000000) >> 32) | ((t->b[0] & 0x00000000FFFFFFFF) << 32);
        t->b[1] = ((t->b[1] & 0xFFFFFFFF00000000) >> 32) | ((t->b[1] & 0x00000000FFFFFFFF) << 32);
    }
    void reverseVerticle(u64& move){
        move = ((move & 0xFF00FF00FF00FF00) >>  8) | ((move & 0x00FF00FF00FF00FF) << 8);
        move = ((move & 0xFFFF0000FFFF0000) >> 16) | ((move & 0x0000FFFF0000FFFF) << 16);
        move = ((move & 0xFFFFFFFF00000000) >> 32) | ((move & 0x00000000FFFFFFFF) << 32);
    }

    void diagCompress(){
        //compress diag
        //board(i, j) -> board_new(i+j, j) if i+j < 8
        //board(i, j) -> board_new(i+j-8, j) if i+j >= 8
        transpose();
        b[0] =   (b[0] & 0x000000000000FFFF) |
                ((b[0] & 0x000000003FFF0000) <<  2) | ((b[0] & 0x00000000C0000000) >> 14) |
                ((b[0] & 0x00000FFF00000000) <<  4) | ((b[0] & 0x0000F00000000000) >> 12) |
                ((b[0] & 0x03FF000000000000) <<  6) | ((b[0] & 0xFC00000000000000) >> 10);
        b[1] =  ((b[1] & 0x00000000000000FF) <<  8) | ((b[1] & 0x000000000000FF00) >> 8) |
                ((b[1] & 0x00000000003F0000) << 10) | ((b[1] & 0x00000000FFC00000) >> 6) |
                ((b[1] & 0x0000000F00000000) << 12) | ((b[1] & 0x0000FFF000000000) >> 4) |
                ((b[1] & 0x0003000000000000) << 14) | ((b[1] & 0xFFFC000000000000) >> 2);
        transpose();
    }
    void diagCompress(Board* t){
        //compress diag
        //board(i, j) -> board_new(i+j, j) if i+j < 8
        //board(i, j) -> board_new(i+j-8, j) if i+j >= 8
        //t: output, diag compress result
        transpose(t);
        t->b[0] =  (t->b[0] & 0x000000000000FFFF) |
                  ((t->b[0] & 0x000000003FFF0000) <<  2) | ((t->b[0] & 0x00000000C0000000) >> 14) |
                  ((t->b[0] & 0x00000FFF00000000) <<  4) | ((t->b[0] & 0x0000F00000000000) >> 12) |
                  ((t->b[0] & 0x03FF000000000000) <<  6) | ((t->b[0] & 0xFC00000000000000) >> 10);
        t->b[1] = ((t->b[1] & 0x00000000000000FF) <<  8) | ((t->b[1] & 0x000000000000FF00) >> 8) |
                  ((t->b[1] & 0x00000000003F0000) << 10) | ((t->b[1] & 0x00000000FFC00000) >> 6) |
                  ((t->b[1] & 0x0000000F00000000) << 12) | ((t->b[1] & 0x0000FFF000000000) >> 4) |
                  ((t->b[1] & 0x0003000000000000) << 14) | ((t->b[1] & 0xFFFC000000000000) >> 2);
        t->transpose();
    }
    void diagCompress(u64& move){
        //compress diag
        //move(i, j) -> move_new(i+j, j) if i+j < 8
        //move(i, j) -> move_new(i+j-8, j) if i+j >= 8
        transpose(move);
        move =  (move & 0x00000000000000FF) |
               ((move & 0x0000000000007F00) << 1) | ((move & 0x0000000000008000) >> 7) |
               ((move & 0x00000000003F0000) << 2) | ((move & 0x0000000000C00000) >> 6) |
               ((move & 0x000000001F000000) << 3) | ((move & 0x00000000E0000000) >> 5) |
               ((move & 0x0000000F00000000) << 4) | ((move & 0x000000F000000000) >> 4) |
               ((move & 0x0000070000000000) << 5) | ((move & 0x0000F80000000000) >> 3) |
               ((move & 0x0003000000000000) << 6) | ((move & 0x00FC000000000000) >> 2) |
               ((move & 0x0100000000000000) << 7) | ((move & 0xFE00000000000000) >> 1);
        transpose(move); 
    }

    void diagCompressInverse(){
        //inverse of compress diag
        //board_new(i, j) <- board(i+j, j) if i+j < 8
        //board_new(i, j) <- board(i+j-8, j) if i+j >= 8
        transpose();
        b[0] =   (b[0] & 0x000000000000FFFF) |
                ((b[0] & 0x00000000FFFC0000) >>  2) | ((b[0] & 0x0000000000030000) << 14) |
                ((b[0] & 0x0000FFF000000000) >>  4) | ((b[0] & 0x0000000F00000000) << 12) |
                ((b[0] & 0xFFC0000000000000) >>  6) | ((b[0] & 0x003F000000000000) << 10);
        b[1] =  ((b[1] & 0x000000000000FF00) >>  8) | ((b[1] & 0x00000000000000FF) <<  8) |
                ((b[1] & 0x00000000FC000000) >> 10) | ((b[1] & 0x0000000003FF0000) <<  6) |
                ((b[1] & 0x0000F00000000000) >> 12) | ((b[1] & 0x00000FFF00000000) <<  4) |
                ((b[1] & 0xC000000000000000) >> 14) | ((b[1] & 0x3FFF000000000000) <<  2);
        transpose();
    }
    void diagCompressInverse(Board* t){
        //inverse of compress diag
        //board_new(i, j) <- board(i+j, j) if i+j < 8
        //board_new(i, j) <- board(i+j-8, j) if i+j >= 8
        //t: output, inverse diag compress result
        transpose(t);
        t->b[0] =  (t->b[0] & 0x000000000000FFFF) |
                  ((t->b[0] & 0x00000000FFFC0000) >>  2) | ((t->b[0] & 0x0000000000030000) << 14) |
                  ((t->b[0] & 0x0000FFF000000000) >>  4) | ((t->b[0] & 0x0000000F00000000) << 12) |
                  ((t->b[0] & 0xFFC0000000000000) >>  6) | ((t->b[0] & 0x003F000000000000) << 10);
        t->b[1] = ((t->b[1] & 0x000000000000FF00) >>  8) | ((t->b[1] & 0x00000000000000FF) <<  8) |
                  ((t->b[1] & 0x00000000FC000000) >> 10) | ((t->b[1] & 0x0000000003FF0000) <<  6) |
                  ((t->b[1] & 0x0000F00000000000) >> 12) | ((t->b[1] & 0x00000FFF00000000) <<  4) |
                  ((t->b[1] & 0xC000000000000000) >> 14) | ((t->b[1] & 0x3FFF000000000000) <<  2);
        t->transpose();
    }
    void diagCompressInverse(u64& move){
        //compress diag
        //move_new(i, j) <- move(i+j, j) if i+j < 8
        //move_new(i, j) <- move(i+j-8, j) if i+j >= 8
        transpose(move);
        move =  (move & 0x00000000000000FF) |
               ((move & 0x000000000000FE00) >> 1) | ((move & 0x0000000000000100) << 7) |
               ((move & 0x0000000000FC0000) >> 2) | ((move & 0x0000000000030000) << 6) |
               ((move & 0x00000000F8000000) >> 3) | ((move & 0x0000000007000000) << 5) |
               ((move & 0x000000F000000000) >> 4) | ((move & 0x0000000F00000000) << 4) |
               ((move & 0x0000E00000000000) >> 5) | ((move & 0x00001F0000000000) << 3) |
               ((move & 0x00C0000000000000) >> 6) | ((move & 0x003F000000000000) << 2) |
               ((move & 0x8000000000000000) >> 7) | ((move & 0x7F00000000000000) << 1);
        transpose(move);
    }

    void checkHorizon(u64& move){
        //if pos can move and flip horizontally, move(pos) = 1 else 0
        //move: can or cannot move and flip horizontally
        move = pre_move[b[0] & 0xFFFF] |
               (pre_move[b[0] >> 16 & 0xFFFF] << 8) |
               (pre_move[b[0] >> 32 & 0xFFFF] << 16) |
               (pre_move[b[0] >> 48 & 0xFFFF] << 24) |
               (pre_move[b[1] & 0xFFFF]       << 32) |
               (pre_move[b[1] >> 16 & 0xFFFF] << 40) |
               (pre_move[b[1] >> 32 & 0xFFFF] << 48) |
               (pre_move[b[1] >> 48 & 0xFFFF] << 56);
    }
    void checkVerticle(u64& move){
        transpose();
        checkHorizon(move);
        transpose(move);
        transpose();
    }
    void checkDiag(u64 &move){
        diagCompress();
        Board diag_tmp;
        diag_tmp.b[0] = (b[0] & 0x55FF557F555F5557 | 0x5500554055505554);
        diag_tmp.b[1] = (b[1] & 0xFFFF7FFF5FFF57FF | 0x0000400050005400);
        diag_tmp.checkHorizon(move);
        diag_tmp.b[0] = (b[0] & 0xFF55FFD5FFF5FFFD | 0x0055001500050001);
        diag_tmp.b[1] = (b[1] & 0x5555D555F555FD55 | 0x5555155505550155);
        u64 move_tmp;
        diag_tmp.checkHorizon(move_tmp);
        move |= move_tmp;
        diagCompressInverse(move);
        diagCompressInverse();
    }
    
    void testMove(u64& move){
        //if pos can move, move(pos) = 1 else 0
        //move: can or cannot move
        move = 0;
        u64 move_tmp;
        checkVerticle(move_tmp);
        move |= move_tmp;
        
        checkHorizon(move_tmp);
        move |= move_tmp;

        checkDiag(move_tmp);
        move |= move_tmp;

        reverseHorizon();
        checkDiag(move_tmp);
        reverseHorizon(move_tmp);
        move |= move_tmp;
        reverseHorizon();
    }
    void splitMove(int* move_ind, int& move_cnt, const u64& move){
        int split_tmp, i, j;
        move_cnt = 0;
        for(i = 0; i < 8; ++i){
            split_tmp = (move>>(i<<3))&0xFF;
            for(j = 0; j < move_pos_cnt[split_tmp]; ++j){
                move_ind[move_cnt++] = (i<<3|move_pos[split_tmp][j]);
            }
        }
    }

    void nextBoard(const int& x, const int& y){
        //board(x, y) = 0 then calculate reversi result, must make sure it is valid
        u64 nxt_ind = (b[x>>2&1] >> ((x&3)<<4)) & 0xFFFF;
        b[x>>2&1] = (b[x>>2&1]^(nxt_ind << ((x&3)<<4))) | (nxt_move[nxt_ind][y] << ((x&3)<<4));

        u64 mask[2];
        mask[0] = (b[0] & (0x0003000300030003 << (y<<1)));
        mask[1] = (b[1] & (0x0003000300030003 << (y<<1)));
        nxt_ind =  ((b[0] >> (y << 1)) & 3) |
                  (((b[0] >> (y << 1|0x10)) &3) <<  2) |
                  (((b[0] >> (y << 1|0x20)) &3) <<  4) |
                  (((b[0] >> (y << 1|0x30)) &3) <<  6) |
                  (((b[1] >> (y << 1) &3)) << 8) |
                  (((b[1] >> (y << 1|0x10)) &3) << 10) |
                  (((b[1] >> (y << 1|0x20)) &3) << 12) |
                  (((b[1] >> (y << 1|0x30)) &3) << 14);
        b[0] = (b[0]^mask[0]) |  ((nxt_move[nxt_ind][x]&3) << (y<<1)) |
                              (((nxt_move[nxt_ind][x]>>2)&3) << (y<<1|0x10)) |
                              (((nxt_move[nxt_ind][x]>>4)&3) << (y<<1|0x20)) |
                              (((nxt_move[nxt_ind][x]>>6)&3) << (y<<1|0x30));
        b[1] = (b[1]^mask[1]) | (((nxt_move[nxt_ind][x]>>8)&3) << (y<<1)) |
                              (((nxt_move[nxt_ind][x]>>10)&3) << (y<<1|0x10)) |
                              (((nxt_move[nxt_ind][x]>>12)&3) << (y<<1|0x20)) |
                              (((nxt_move[nxt_ind][x]>>14)&3) << (y<<1|0x30));

        mask[0] = mask[1] = 0;
        int down = std::max(0, x+y-7);
        int up = std::min(x+y, 7);
        nxt_ind = 0x5555;
        for(int i = down; i <= up; ++i){
            mask[i>>2&1] |= ((u64)(0x3) << ((x+y-i)<<1) << ((i&3) << 4));
            nxt_ind = (nxt_ind ^ ((0x1) << (i<<1))) | (((b[i>>2&1] >> ((x+y-i)<<1) >> ((i&3) << 4))&3) << (i<<1));
        }
        b[0] = (b[0]^(b[0] & mask[0]));
        b[1] = (b[1]^(b[1] & mask[1]));
        nxt_ind = nxt_move[nxt_ind][x];
        for(int i = down; i <= up; ++i){
            b[i>>2&1] |= ((nxt_ind >> (i<<1))&3) << ((x+y-i)<<1) << ((i&3) << 4);
        }

        mask[0] = mask[1] = 0;
        down = std::max(0, x-y);
        up = std::min(7, 7+x-y);
        nxt_ind = 0x5555;
        for(int i = down; i <= up; ++i){
            mask[i>>2&1] |= ((u64)(0x3) << ((y-x+i)<<1) << ((i&3) << 4));
            nxt_ind = (nxt_ind ^ ((0x1) << (i<<1))) | (((b[i>>2&1] >> ((y-x+i)<<1) >> ((i&3) << 4))&3) << (i<<1));
        }
        b[0] = (b[0]^(b[0] & mask[0]));
        b[1] = (b[1]^(b[1] & mask[1]));
        nxt_ind = nxt_move[nxt_ind][x];
        for(int i = down; i <= up; ++i){
            b[i>>2&1] |= ((nxt_ind >> (i<<1))&3) << ((y-x+i)<<1) << ((i&3) << 4);
        }
        b[x>>2&1] ^= ((u64)(0x0000000000000002) << ((((x&3)<<3)|y)<<1));
    }
    void nextBoard(Board* t, const int& x, const int& y){
        t->b[0] = b[0]; t->b[1] = b[1];
        t->nextBoard(x, y);
    }

    void calPattern88(int* p_sum){
        //p_sum [0, 88)
        calPattern(p_sum);

        reverseVerticle();
        calPattern(p_sum+p_max);

        reverseHorizon();
        calPattern(p_sum+((p_max<<1)+p_max));

        reverseVerticle();
        calPattern(p_sum+(p_max<<1));

        transpose();
        calPattern(p_sum+((p_max<<2)+p_max));

        reverseHorizon();
        calPattern(p_sum+((p_max<<2)+(p_max<<1)+p_max));

        reverseVerticle();
        calPattern(p_sum+((p_max<<2)+(p_max<<1)));

        reverseHorizon();
        calPattern(p_sum+(p_max<<2));

        transpose();
    }

    void calPattern46(int* p_sum){
        //p_sum [0, 46)
        //horizon
        p_sum[0] = base3[(b[0]&0xFFFF)|
                         (((b[0]>>18)&0x3)<<16)|
                         (((b[0]>>28)&0x3)<<18)];
        p_sum[1] = base3[(b[0]>>16)&0xFFFF];
        p_sum[2] = base3[(b[0]>>32)&0xFFFF];
        p_sum[3] = base3[(b[0]>>48)&0xFFFF];
        p_sum[4] = base3[b[1]&0xFFFF];
        p_sum[5] = base3[(b[1]>>16)&0xFFFF];
        p_sum[6] = base3[(b[1]>>32)&0xFFFF];
        p_sum[7] = base3[((b[1]>>48)&0xFFFF)|
                         (((b[1]>>34)&0x3)<<16)|
                         (((b[1]>>44)&0x3)<<18)];

        //Vertical
        transpose();
        p_sum[8] = base3[(b[0]&0xFFFF)|
                         (((b[0]>>18)&0x3)<<16)|
                         (((b[0]>>28)&0x3)<<18)];
        p_sum[9] = base3[(b[0]>>16)&0xFFFF];
        p_sum[10] = base3[(b[0]>>32)&0xFFFF];
        p_sum[11] = base3[(b[0]>>48)&0xFFFF];
        p_sum[12] = base3[b[1]&0xFFFF];
        p_sum[13] = base3[(b[1]>>16)&0xFFFF];
        p_sum[14] = base3[(b[1]>>32)&0xFFFF];
        p_sum[15] = base3[((b[1]>>48)&0xFFFF)|
                         (((b[1]>>34)&0x3)<<16)|
                         (((b[1]>>44)&0x3)<<18)];
        transpose();

        //diag
        diagCompress();
        p_sum[16] = base3[(b[0]>>48)&0xFF];
        p_sum[17] = base3[b[1]&0x3FF];
        p_sum[18] = base3[(b[1]>>16)&0xFFF];
        p_sum[19] = base3[(b[1]>>32)&0x3FFF];
        p_sum[20] = base3[(b[1]>>48)&0xFFFF];
        p_sum[21] = base3[b[0]>>56];
        p_sum[22] = base3[(b[0]>>38)&0x3FF];
        p_sum[23] = base3[(b[0]>>20)&0xFFF];
        p_sum[24] = base3[(b[0]>>2)&0x3FFF];

        p_sum[42] = base3[(b[0]&0x3FF)|
                     (((b[0]>>16)&0x3FF)<<10)];
        reverseHorizon();
        p_sum[43] = base3[(b[0]&0x3FF)|
                     (((b[0]>>16)&0x3FF)<<10)];
        reverseVerticle();
        p_sum[45] = base3[(b[0]&0x3FF)|
                     (((b[0]>>16)&0x3FF)<<10)];
        reverseHorizon();
        p_sum[44] = base3[(b[0]&0x3FF)|
                     (((b[0]>>16)&0x3FF)<<10)];
        reverseVerticle();

        diagCompressInverse();

        reverseHorizon();
        p_sum[35] = base3[(b[0]&0x3F)|
                     (((b[0]>>16)&0x3F)<<6)|
                     (((b[0]>>32)&0x3F)<<12)];
        p_sum[39] = base3[(b[0]&0x3FF)|
                     (((b[0]>>16)&0x3FF)<<10)];
        diagCompress();
        p_sum[25] = base3[(b[0]>>48)&0xFF];
        p_sum[26] = base3[b[1]&0x3FF];
        p_sum[27] = base3[(b[1]>>16)&0xFFF];
        p_sum[28] = base3[(b[1]>>32)&0x3FFF];
        p_sum[29] = base3[(b[1]>>48)&0xFFFF];
        p_sum[30] = base3[b[0]>>56];
        p_sum[31] = base3[(b[0]>>38)&0x3FF];
        p_sum[32] = base3[(b[0]>>20)&0xFFF];
        p_sum[33] = base3[(b[0]>>2)&0x3FFF];
        diagCompressInverse();

        reverseVerticle();
        p_sum[37] = base3[(b[0]&0x3F)|
                     (((b[0]>>16)&0x3F)<<6)|
                     (((b[0]>>32)&0x3F)<<12)];
        p_sum[40] = base3[(b[0]&0x3FF)|
                     (((b[0]>>16)&0x3FF)<<10)];
        reverseHorizon();
        p_sum[36] = base3[(b[0]&0x3F)|
                     (((b[0]>>16)&0x3F)<<6)|
                     (((b[0]>>32)&0x3F)<<12)];
        p_sum[41] = base3[(b[0]&0x3FF)|
                     (((b[0]>>16)&0x3FF)<<10)];
        reverseVerticle();

        //3x3
        p_sum[34] = base3[(b[0]&0x3F)|
                     (((b[0]>>16)&0x3F)<<6)|
                     (((b[0]>>32)&0x3F)<<12)];
        
        //2x5
        p_sum[38] = base3[(b[0]&0x3FF)|
                     (((b[0]>>16)&0x3FF)<<10)];
    }

    int getKey(){
        return ((b[0])^(b[0]>>1)^
               (b[0]>>16)^(b[0]>>19)^
               (b[0]>>32)^(b[0]>>37)^
               (b[0]>>48)^(b[0]>>47)^
               (b[1])^(b[1]>>1)^
               (b[1]>>16)^(b[1]>>19)^
               (b[1]>>32)^(b[1]>>37)^
               (b[1]>>48)^(b[1]>>47))&0xFFFFF;
    }

private:
    inline static int lowbit(const int& u){
        //return the lowest 1 in u(2-based)
        //watch out! the type of u is int!
        return (u&(-u));
    }
    inline int popCount(u64 u){
        //count number of 1 in u(2-based)
        u = (u & 0x5555555555555555) + ((u>>1)  & 0x5555555555555555);
        u = (u & 0x3333333333333333) + ((u>>2)  & 0x3333333333333333);
        u = (u & 0x0F0F0F0F0F0F0F0F) + ((u>>4)  & 0x0F0F0F0F0F0F0F0F);
        u = (u & 0x00FF00FF00FF00FF) + ((u>>8)  & 0x00FF00FF00FF00FF);
        u = (u & 0x0000FFFF0000FFFF) + ((u>>16) & 0x0000FFFF0000FFFF);
        u = (u & 0x00000000FFFFFFFF) + ((u>>32) & 0x00000000FFFFFFFF);
        return u;
    }
    void calPattern(int* p_sum){
        p_sum[0] = base3[(b[0]&0x3F)|
                         (((b[0]>>16)&0x3F)<<6)|
                         (((b[0]>>32)&0x3F)<<12)];
        p_sum[1] = base3[(b[0]&0x3FF)|
                         (((b[0]>>16)&0x3FF)<<10)];
        p_sum[2] = base3[(b[0]&0xFFFF)|
                         (((b[0]>>18)&0x3)<<16)|
                         (((b[0]>>28)&0x3)<<18)];
        p_sum[3] = base3[(b[0]>>48)];
        p_sum[4] = base3[(b[0]>>32)&0xFFFF];
        p_sum[5] = base3[(b[0]>>16)&0xFFFF];
        diagCompress();
        p_sum[6] = base3[(b[1]>>48)&0xFFFF];
        p_sum[7] = base3[(b[1]>>32)&0x3FFF];
        p_sum[8] = base3[(b[1]>>16)&0xFFF];
        p_sum[9] = base3[b[1]&0x3FF];
        p_sum[10] = base3[(b[0]>>48)&0xFF];
        diagCompressInverse();
    }
};

u64 Board::pre_move[1<<16] = {0};
u64 Board::nxt_move[1<<16][8] = {0};
int Board::base3[1<<20] = {0};
int Board::move_pos[1<<8][8] = {0};
int Board::move_pos_cnt[1<<8] = {0};

#endif