#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <ctime>
#include <random>

//#define _USE_MATH_DEFINES 
#include <cmath>
#include <assert.h>
#include "board.hpp"

//#define DEBUG
const int p_max = 11;
const int depth_para[64] = {0, 0, 0, 0, 9, 9, 9, 9,
                            8, 8, 8, 8, 8, 8, 8, 8,
                            8, 8, 8, 8, 8, 8, 8, 8,
                            8, 8, 8, 8, 8, 8, 8, 8,
                            8, 8, 8, 8, 8, 8, 8, 8,
                            8, 8, 9, 9, 9, 9,10,10,
                           10,10,10,10,12,12,12,12,
                           12,12,12,12,12,12,12,12};
int sort_limit;
float w[16][p_max][59049];
void sendLen() {
    for(int i = 0; i < 3; i++)printf("%c", (unsigned char)(0));
    printf("%c", (unsigned char)(2));
}

void init(){
    Board::init();
    int tmp, n;
    for(int u = 1; u < 16; ++u){
        FILE* fp = fopen(("./model/"+std::to_string(u)+".txt").c_str(), "r");
        for(int i = 0; i < p_max; ++i){
            fscanf(fp, "%d%d", &tmp, &n);
            for(int j = 0; j < n; ++j)fscanf(fp, "%f", &w[u][i][j]);
        }
        fclose(fp);
    }
}

float calValue(Board* t, int step){
    static int p_sum[46];
    t->calPattern46(p_sum);
    float value = 0;
    step = (step>>2);
    value =     w[step][0][p_sum[34]]+
                w[step][0][p_sum[35]]+
                w[step][0][p_sum[36]]+
                w[step][0][p_sum[37]]+
                w[step][1][p_sum[38]]+
                w[step][1][p_sum[39]]+
                w[step][1][p_sum[40]]+
                w[step][1][p_sum[41]]+
                w[step][1][p_sum[42]]+
                w[step][1][p_sum[43]]+
                w[step][1][p_sum[44]]+
                w[step][1][p_sum[45]]+
                w[step][2][p_sum[0]]+
                w[step][2][p_sum[7]]+
                w[step][2][p_sum[8]]+
                w[step][2][p_sum[15]]+
                w[step][3][p_sum[3]]+
                w[step][3][p_sum[4]]+
                w[step][3][p_sum[11]]+
                w[step][3][p_sum[12]]+
                w[step][4][p_sum[2]]+
                w[step][4][p_sum[5]]+
                w[step][4][p_sum[10]]+
                w[step][4][p_sum[13]]+
                w[step][5][p_sum[1]]+
                w[step][5][p_sum[6]]+
                w[step][5][p_sum[9]]+
                w[step][5][p_sum[14]]+
                w[step][6][p_sum[20]]+
                w[step][6][p_sum[29]]+
                w[step][7][p_sum[19]]+
                w[step][7][p_sum[24]]+
                w[step][7][p_sum[28]]+
                w[step][7][p_sum[33]]+
                w[step][8][p_sum[18]]+
                w[step][8][p_sum[23]]+
                w[step][8][p_sum[27]]+
                w[step][8][p_sum[32]]+
                w[step][9][p_sum[17]]+
                w[step][9][p_sum[22]]+
                w[step][9][p_sum[26]]+
                w[step][9][p_sum[31]]+
                w[step][10][p_sum[16]]+
                w[step][10][p_sum[21]]+
                w[step][10][p_sum[25]]+
                w[step][10][p_sum[30]];
    return value;
}

bool getAns(Board* t, int& nxt_x, int& nxt_y, 
            int depth, int time_end){
    //stack
    int top;//当前栈顶
    Board state[64];//当前栈帧的状态
    int move_ind[64][64], move_cnt[64], move_iter[64];//当前栈帧的可行移动
    float mx_value[64];//当前栈帧的最大值
    bool nega[64];//是否取负向上传递
    //tmp_var
    u64 move;
    int num[2];
    float value_tmp;
    //init
    t->calNum(num);
    int chess_num = num[0]+num[1];

    if(chess_num+depth > 64)return false;
    //depth_limit
    int mx_depth = depth;
    sort_limit = mx_depth-2;
    int end_chess_num = mx_depth+chess_num;

    top = 0;
    state[top].b[0] = t->b[0]; state[top].b[1] = t->b[1];

    int final_choice = -1;

    state[top].testMove(move);
    state[top].splitMove(move_ind[top], move_cnt[top], move);

    mx_value[top] = -1e12;
    move_iter[top] = 0;
    nega[top] = false;

    //sort first step
    std::pair<float, int>move_sort[64];
    Board nxt_sort;
    for(int i = 0; i < move_cnt[top]; ++i){
        move_sort[i].second = move_ind[top][i];
        state[top].nextBoard(&nxt_sort, move_ind[top][i]>>3, move_ind[top][i]&7);
        nxt_sort.reverse();
        nxt_sort.testMove(move);
        if(!move){
            nxt_sort.reverse();
            nxt_sort.testMove(move);
            if(!move){
                nxt_sort.calNum(num);
                //move_sort[i].first = num[0]-num[1];
                if(num[0]-num[1] > mx_value[top]){
                    mx_value[top] = num[0]-num[1];
                    final_choice = move_ind[top][i];
                }
                move_sort[i].first = 1e12;
                move_iter[top]++;
            }
            else{
                move_sort[i].first = calValue(&nxt_sort, chess_num+1);
            }
        }
        else{
            move_sort[i].first = -calValue(&nxt_sort, chess_num+1);
        }
    }
    sort(move_sort, move_sort+move_cnt[top]);
    for(int i = 0; i < move_cnt[top]; ++i){
        move_ind[top][move_cnt[top]-i-1] = move_sort[i].second;
    }

    if(mx_depth == 1){
        assert(final_choice != -1);
        nxt_x = (final_choice>>3);
        nxt_y = (final_choice&7);
        std::cerr << mx_value[0] << ' ' << nxt_x << ' ' << nxt_y << ' ' << clock() << ' ' 
            << time_end << std::endl;
        return true;
    }

    int node_cnt = 1;

    while(top != -1){
        if(move_iter[top] == move_cnt[top]){
            if(clock() > time_end)return false;
            if(top == 0)--top;
            else{
                if(nega[top]){
                    if(-mx_value[top] > mx_value[top-1]){
                        mx_value[top-1] = -mx_value[top];
                        if(top == 1)final_choice = move_ind[0][move_iter[0]-1];
                        else if(mx_value[top] <= mx_value[top-2] && nega[top-1])--top;
                    }
                }
                else{
                    if(mx_value[top] > mx_value[top-1]){
                        mx_value[top-1] = mx_value[top];
                        if(top == 1)final_choice = move_ind[0][move_iter[0]-1];
                        else if(-mx_value[top] <= mx_value[top-2] && nega[top-1])--top;
                    }
                }
                --top;
            }
        }
        else{
            ++node_cnt;
            state[top].nextBoard(&state[top+1], move_ind[top][move_iter[top]]>>3, move_ind[top][move_iter[top]]&7);
            move_iter[top]++;
            ++top;
            state[top].reverse();
            state[top].testMove(move);
            if(!move){
                state[top].reverse();
                state[top].testMove(move);
                if(!move){
                    state[top].calNum(num);
                    --top;
                    if(num[0]-num[1] > mx_value[top]){
                        mx_value[top] = num[0]-num[1];
                        if(top == 0)final_choice = move_ind[0][move_iter[0]-1];
                        else if(-mx_value[top] <= mx_value[top-1] && nega[top])--top;
                    }
                    continue;
                }
                nega[top] = false;
            }
            else nega[top] = true;
            if(top == mx_depth){
                value_tmp = calValue(&state[top], end_chess_num);
                if(nega[top])mx_value[top-1] = std::fmax(mx_value[top-1], -value_tmp);
                else mx_value[top-1] = std::fmax(mx_value[top-1], value_tmp);
                --top;
                if(-mx_value[top] < mx_value[top-1] && nega[top])--top;
            }
            else{
                state[top].splitMove(move_ind[top], move_cnt[top], move);
                mx_value[top] = -1e12;
                move_iter[top] = 0;
                if(top <= sort_limit){
                    for(int i = 0; i < move_cnt[top]; ++i){
                        move_sort[i].second = move_ind[top][i];
                        state[top].nextBoard(&nxt_sort, move_ind[top][i]>>3, move_ind[top][i]&7);
                        nxt_sort.reverse();
                        nxt_sort.testMove(move);
                        if(!move){
                            nxt_sort.reverse();
                            nxt_sort.testMove(move);
                            if(!move){
                                nxt_sort.calNum(num);
                                mx_value[top] = std::fmax(mx_value[top], num[0]-num[1]);
                                move_sort[i].first = 1e12;
                                move_iter[top]++;
                            }
                            else{
                                move_sort[i].first = calValue(&nxt_sort, chess_num+top+1);
                            }
                        }
                        else{
                            move_sort[i].first = -calValue(&nxt_sort, chess_num+top+1);
                        }
                    }
                    sort(move_sort, move_sort+move_cnt[top]);
                    for(int i = 0; i < move_cnt[top]; ++i){
                        move_ind[top][move_cnt[top]-i-1] = move_sort[i].second;
                    }
                }
            }
        }
    }
    assert(final_choice != -1);
    nxt_x = (final_choice>>3);
    nxt_y = (final_choice&7);
    std::cerr << mx_value[0] << ' ' << nxt_x << ' ' << nxt_y << ' ' << clock() << ' ' 
        << time_end << std::endl;
    return true;
}


int main(){
    char state_str[68];
    int board_init[64];
    int nxt_x, nxt_y;
    int ans_x, ans_y;
    int player_now;
    int time_end;
    int test_depth;
    int num[2];
    Board b_init;

    bool is_start = true;

    while(true){
        fgets(state_str, 68, stdin);
        time_end = clock()+2.8*CLOCKS_PER_SEC;
        if(is_start){
            init();
            is_start = false;
        }
        player_now = state_str[1]-'0';
        std::cerr << state_str << std::endl;
        for(int i = 0; i < 64; ++i)board_init[i] = state_str[i+2]-'0';
        b_init.encode(board_init);
        if(player_now)b_init.reverse();
        b_init.calNum(num);
        test_depth = std::min(8, 64-num[0]-num[1]);
        while(getAns(&b_init, ans_x, ans_y, test_depth, time_end)){
            nxt_x = ans_x;
            nxt_y = ans_y;
            test_depth++;
        }
        std::cerr << "depth = " << test_depth << std::endl;
        sendLen();
        std::cout << std::to_string(nxt_x)+std::to_string(nxt_y);
        std::cout.flush();
    }return 0;
}