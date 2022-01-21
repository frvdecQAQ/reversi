//make pattern value

#include <iostream>
#include <cstdio>
#include <ctime>
#include <assert.h>
#include <cmath>
#include "board.hpp"

int batch = 65536;
int batch_test = 4096;
int epoch = 100;
const int inf = 0x3fffffff/3;
const int p_max = 11;
const int POW[11] = {1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049};
const int pattern_sum[p_max] = {POW[9], POW[10], POW[10], POW[8], POW[8],
                                POW[8], POW[8], POW[7], POW[6], POW[5], POW[4]};
float w_last[p_max][59049] = {0.0};
float w[p_max][59049] = {0.0};// [-200, 200]
int board_init[64];
float lr_decay = 0.995;
float lr = 1e-3;
int base_step;
int train_cnt = 0;

void printBoard(Board* t){
    int b[64];
    t->decode(b);
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            assert(b[i<<3|j] == 1 || b[i<<3|j] == 2 || b[i<<3|j] == 0);
            if(b[i<<3|j] == 2)std::cout << "+ ";
            else if(b[i<<3|j] == 0)std::cout << "o ";
            else std::cout << "x ";
        }std::cout << std::endl;
    }std::cout << std::endl;
}

void input_w(int n){
    FILE* fp = fopen(("./model/"+std::to_string(n)+".txt").c_str(), "r");
    int flag, a, b;
    for(int i = 0; i < p_max; ++i){
        flag = fscanf(fp, "%d%d", &a, &b);
        assert(flag);
        assert(a == i);
        assert(b == pattern_sum[i]);
        for(int j = 0; j < pattern_sum[i]; ++j){
            flag = fscanf(fp, "%f", &w_last[i][j]);
            assert(flag);
        }
    }
    fclose(fp);
}

void output_w(int n){
    FILE* fp = fopen(("./model/"+std::to_string(n)+".txt").c_str(), "w");
    for(int i = 0; i < p_max; ++i){
        fprintf(fp, "%d %d\n", i, pattern_sum[i]);
        for(int j = 0; j < pattern_sum[i]; ++j){
            fprintf(fp, "%.6f ", w[i][j]);
            if((j+1)%50 == 0)fprintf(fp, "\n");
        }fprintf(fp, "\n");
    }
    fclose(fp);
}

bool genBoard(Board* t, int step){
    int now_step = 4;
    u64 move;
    int move_ind[64], move_cnt = 0;
    int choice;

    t->testMove(move);
    while(now_step < step){
        t->splitMove(move_ind, move_cnt, move);
        choice = rand()%move_cnt;
        choice = move_ind[choice];
        t->nextBoard(choice>>3, choice&7);
        t->reverse();
        t->testMove(move);
        if(!move){
            t->reverse();
            t->testMove(move);
            if(!move)return false;
        }
        now_step++;
    }return true;
}

float getValueLast(Board* t){
    static int p_sum[46];
    t->calPattern46(p_sum);
    float value_hat = 0;
    /*for(int i = 0; i < p_max; ++i){
        for(int j = i; j < (p_max<<3); j += p_max){
            value_hat += w_last[i][p_sum[j]];
        }
    }*/
    value_hat = w_last[0][p_sum[34]]+
                w_last[0][p_sum[35]]+
                w_last[0][p_sum[36]]+
                w_last[0][p_sum[37]]+

                w_last[1][p_sum[38]]+
                w_last[1][p_sum[39]]+
                w_last[1][p_sum[40]]+
                w_last[1][p_sum[41]]+
                w_last[1][p_sum[42]]+
                w_last[1][p_sum[43]]+
                w_last[1][p_sum[44]]+
                w_last[1][p_sum[45]]+

                w_last[2][p_sum[0]]+
                w_last[2][p_sum[7]]+
                w_last[2][p_sum[8]]+
                w_last[2][p_sum[15]]+

                w_last[3][p_sum[3]]+
                w_last[3][p_sum[4]]+
                w_last[3][p_sum[11]]+
                w_last[3][p_sum[12]]+

                w_last[4][p_sum[2]]+
                w_last[4][p_sum[5]]+
                w_last[4][p_sum[10]]+
                w_last[4][p_sum[13]]+

                w_last[5][p_sum[1]]+
                w_last[5][p_sum[6]]+
                w_last[5][p_sum[9]]+
                w_last[5][p_sum[14]]+

                w_last[6][p_sum[20]]+
                w_last[6][p_sum[29]]+

                w_last[7][p_sum[19]]+
                w_last[7][p_sum[24]]+
                w_last[7][p_sum[28]]+
                w_last[7][p_sum[33]]+

                w_last[8][p_sum[18]]+
                w_last[8][p_sum[23]]+
                w_last[8][p_sum[27]]+
                w_last[8][p_sum[32]]+

                w_last[9][p_sum[17]]+
                w_last[9][p_sum[22]]+
                w_last[9][p_sum[26]]+
                w_last[9][p_sum[31]]+

                w_last[10][p_sum[16]]+
                w_last[10][p_sum[21]]+
                w_last[10][p_sum[25]]+
                w_last[10][p_sum[30]];
    
    return value_hat;
}

float getValueHat(Board* t){
    static int p_sum[46];
    //u64 b0 = t->b[0];
    //u64 b1 = t->b[1];
    t->calPattern46(p_sum);
    //assert(b0 == t->b[0]);
    //assert(b1 == t->b[1]);
    float value_hat = 0;
    /*for(int i = 0; i < p_max; ++i){
        for(int j = i; j < (p_max<<3); j += p_max){
            value_hat += w[i][p_sum[j]];
        }
    }*/
    value_hat = w[0][p_sum[34]]+
                w[0][p_sum[35]]+
                w[0][p_sum[36]]+
                w[0][p_sum[37]]+

                w[1][p_sum[38]]+
                w[1][p_sum[39]]+
                w[1][p_sum[40]]+
                w[1][p_sum[41]]+
                w[1][p_sum[42]]+
                w[1][p_sum[43]]+
                w[1][p_sum[44]]+
                w[1][p_sum[45]]+

                w[2][p_sum[0]]+
                w[2][p_sum[7]]+
                w[2][p_sum[8]]+
                w[2][p_sum[15]]+

                w[3][p_sum[3]]+
                w[3][p_sum[4]]+
                w[3][p_sum[11]]+
                w[3][p_sum[12]]+

                w[4][p_sum[2]]+
                w[4][p_sum[5]]+
                w[4][p_sum[10]]+
                w[4][p_sum[13]]+

                w[5][p_sum[1]]+
                w[5][p_sum[6]]+
                w[5][p_sum[9]]+
                w[5][p_sum[14]]+

                w[6][p_sum[20]]+
                w[6][p_sum[29]]+

                w[7][p_sum[19]]+
                w[7][p_sum[24]]+
                w[7][p_sum[28]]+
                w[7][p_sum[33]]+

                w[8][p_sum[18]]+
                w[8][p_sum[23]]+
                w[8][p_sum[27]]+
                w[8][p_sum[32]]+

                w[9][p_sum[17]]+
                w[9][p_sum[22]]+
                w[9][p_sum[26]]+
                w[9][p_sum[31]]+

                w[10][p_sum[16]]+
                w[10][p_sum[21]]+
                w[10][p_sum[25]]+
                w[10][p_sum[30]];
    return value_hat;
}

void train(Board* t, const float& value){
    train_cnt++;
    static int p_sum[46];
    t->calPattern46(p_sum);
    float value_hat = 0;
    value_hat = w[0][p_sum[34]]+
                w[0][p_sum[35]]+
                w[0][p_sum[36]]+
                w[0][p_sum[37]]+

                w[1][p_sum[38]]+
                w[1][p_sum[39]]+
                w[1][p_sum[40]]+
                w[1][p_sum[41]]+
                w[1][p_sum[42]]+
                w[1][p_sum[43]]+
                w[1][p_sum[44]]+
                w[1][p_sum[45]]+

                w[2][p_sum[0]]+
                w[2][p_sum[7]]+
                w[2][p_sum[8]]+
                w[2][p_sum[15]]+

                w[3][p_sum[3]]+
                w[3][p_sum[4]]+
                w[3][p_sum[11]]+
                w[3][p_sum[12]]+

                w[4][p_sum[2]]+
                w[4][p_sum[5]]+
                w[4][p_sum[10]]+
                w[4][p_sum[13]]+

                w[5][p_sum[1]]+
                w[5][p_sum[6]]+
                w[5][p_sum[9]]+
                w[5][p_sum[14]]+

                w[6][p_sum[20]]+
                w[6][p_sum[29]]+

                w[7][p_sum[19]]+
                w[7][p_sum[24]]+
                w[7][p_sum[28]]+
                w[7][p_sum[33]]+

                w[8][p_sum[18]]+
                w[8][p_sum[23]]+
                w[8][p_sum[27]]+
                w[8][p_sum[32]]+

                w[9][p_sum[17]]+
                w[9][p_sum[22]]+
                w[9][p_sum[26]]+
                w[9][p_sum[31]]+

                w[10][p_sum[16]]+
                w[10][p_sum[21]]+
                w[10][p_sum[25]]+
                w[10][p_sum[30]];
    w[0][p_sum[34]]-= lr*(value_hat-value);
    w[0][p_sum[35]]-= lr*(value_hat-value);
    w[0][p_sum[36]]-= lr*(value_hat-value);
    w[0][p_sum[37]]-= lr*(value_hat-value);
    w[1][p_sum[38]]-= lr*(value_hat-value);
    w[1][p_sum[39]]-= lr*(value_hat-value);
    w[1][p_sum[40]]-= lr*(value_hat-value);
    w[1][p_sum[41]]-= lr*(value_hat-value);
    w[1][p_sum[42]]-= lr*(value_hat-value);
    w[1][p_sum[43]]-= lr*(value_hat-value);
    w[1][p_sum[44]]-= lr*(value_hat-value);
    w[1][p_sum[45]]-= lr*(value_hat-value);
     w[2][p_sum[0]]-= lr*(value_hat-value);
     w[2][p_sum[7]]-= lr*(value_hat-value);
     w[2][p_sum[8]]-= lr*(value_hat-value);
    w[2][p_sum[15]]-= lr*(value_hat-value);
     w[3][p_sum[3]]-= lr*(value_hat-value);
     w[3][p_sum[4]]-= lr*(value_hat-value);
    w[3][p_sum[11]]-= lr*(value_hat-value);
    w[3][p_sum[12]]-= lr*(value_hat-value);
     w[4][p_sum[2]]-= lr*(value_hat-value);
     w[4][p_sum[5]]-= lr*(value_hat-value);
    w[4][p_sum[10]]-= lr*(value_hat-value);
    w[4][p_sum[13]]-= lr*(value_hat-value);
     w[5][p_sum[1]]-= lr*(value_hat-value);
     w[5][p_sum[6]]-= lr*(value_hat-value);
     w[5][p_sum[9]]-= lr*(value_hat-value);
    w[5][p_sum[14]]-= lr*(value_hat-value);
    w[6][p_sum[20]]-= lr*(value_hat-value);
    w[6][p_sum[29]]-= lr*(value_hat-value);
    w[7][p_sum[19]]-= lr*(value_hat-value);
    w[7][p_sum[24]]-= lr*(value_hat-value);
    w[7][p_sum[28]]-= lr*(value_hat-value);
    w[7][p_sum[33]]-= lr*(value_hat-value);
    w[8][p_sum[18]]-= lr*(value_hat-value);
    w[8][p_sum[23]]-= lr*(value_hat-value);
    w[8][p_sum[27]]-= lr*(value_hat-value);
    w[8][p_sum[32]]-= lr*(value_hat-value);
    w[9][p_sum[17]]-= lr*(value_hat-value);
    w[9][p_sum[22]]-= lr*(value_hat-value);
    w[9][p_sum[26]]-= lr*(value_hat-value);
    w[9][p_sum[31]]-= lr*(value_hat-value);
   w[10][p_sum[16]]-= lr*(value_hat-value);
   w[10][p_sum[21]]-= lr*(value_hat-value);
   w[10][p_sum[25]]-= lr*(value_hat-value);
   w[10][p_sum[30]]-= lr*(value_hat-value);
}

float getValue(Board* t, int step, bool need_train = false){
    u64 move;
    t->testMove(move);
    if(move == 0){
        int v[2];
        t->calNum(v);
        return v[0]-v[1];
    }
    if(step == base_step+4)return getValueLast(t);

    int move_ind[64], move_cnt = 0;
    t->splitMove(move_ind, move_cnt, move);

    u64 b_tmp[2];
    b_tmp[0] = t->b[0]; b_tmp[1] = t->b[1];
    int now_value = -inf;
    int nxt_value;
    for(int i = 0; i < move_cnt; ++i){
        t->nextBoard(move_ind[i]>>3, move_ind[i]&7);
        t->reverse();
        t->testMove(move);
        if(!move){
            t->reverse();
            nxt_value = getValue(t, step+1, need_train);
            now_value = std::max(nxt_value, now_value);
        }
        else{
            nxt_value = -getValue(t, step+1, need_train);
            now_value = std::max(nxt_value, now_value);
        }
        t->b[0] = b_tmp[0]; t->b[1] = b_tmp[1];
    }
    if(need_train){
        train(t, now_value);
        //u64 b0 = t->b[0];
        //u64 b1 = t->b[1];
        t->transpose();
        train(t, now_value);
        t->reverseHorizon();
        t->reverseVerticle();
        train(t, now_value);
        t->transpose();
        train(t, now_value);
        t->reverseHorizon();
        t->reverseVerticle();
        //assert(b0 == t->b[0]);
        //assert(b1 == t->b[1]);
    }
    return now_value;
}

int main(){
    srand((unsigned)time(NULL));
    Board::init();
    for(int i = 0; i < 64; ++i)board_init[i] = 2;
    board_init[3<<3|3] = board_init[4<<3|4] = 1;
    board_init[3<<3|4] = board_init[4<<3|3] = 0;
    Board base;

    for(int u = 32; u >= 4; u -= 4){
        if(u == 60){
            epoch = 100;
            batch = 65536;
            batch_test = 4096;
            lr = 1e-3;
            lr_decay = 0.97;
        }
        else{
            input_w((u+4)/4);
            epoch = 3;
            batch = 2048;
            batch_test = 256;
            lr = 1e-5;
            lr_decay = 0.95;
        }
        std::cout << "=============== u = " << u << " ===============" << std::endl;
        for(int i = 0; i < p_max; ++i)for(int j = 0; j < pattern_sum[i]; ++j){
            w[i][j] = w_last[i][j];
        }
        base_step = u;
        for(int i = 0; i < epoch; ++i){
            train_cnt = 0;
            for(int j = 0; j < batch; ++j){
                base.encode(board_init);
                while(!genBoard(&base, base_step)){
                    base.encode(board_init);
                }
                float value = getValue(&base, base_step, true);
            }
            lr *= lr_decay;
            float loss = 0;
            float abs_loss = 0;
            for(int j = 0; j < batch_test; ++j){
                int step = rand()%4+base_step;
                base.encode(board_init);
                while(!genBoard(&base, step)){
                    base.encode(board_init);
                }
                float value = getValue(&base, step, false);
                float value_hat = getValueHat(&base);
                loss += 0.5f*(value_hat-value)*(value_hat-value);
                abs_loss += std::fabs(value_hat-value);
            }
            float avg_loss = loss/batch_test;
            abs_loss /= batch_test;
            std::cout << "epoch = " << i << ", avg_loss = " << avg_loss
                << ", abs_loss = " << abs_loss <<  ", cnt = " << train_cnt << std::endl;
        }
        output_w(u/4);
    }
    return 0;
}
