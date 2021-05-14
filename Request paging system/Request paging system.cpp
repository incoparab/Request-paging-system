#include <iostream>
#include <stdlib.h>
#include <stack>
#include <time.h>
#define num 320
using namespace std;
struct FIFO
{
    int n;
    FIFO *next;
    FIFO() { next = NULL; }
    FIFO(int m)
    {
        n = m;
        next = NULL;
    }
};

class RPS
{
private:
    int block_n;   //分配的主存块数
    int F_block_m; //当前已占用内存块数(FIFO)
    int L_block_m; //当前已占用内存块数(LRU)
    int o_block_m; //当前已占用内存块数(OPT)
    FIFO *F_first; //先进先出头指针
    int *LRU;      //用数组模拟栈完成LRU
    int *OPT;      //用数组存储
    int *c_OPT;    //OPT辅助数组
    int F_num;
    int L_num;
    int O_num;

public:
    RPS()
    {
        n_block();
        L_block_m = 0;
        F_block_m = 0;
        o_block_m = 0;
        F_num = 0;
        L_num = 0;
        O_num = 0;
        F_first = new FIFO;
        LRU = new int[block_n];
        OPT = new int[block_n];
        c_OPT = new int[block_n];
        for (int i = 0; i < block_n; i++)
        {
            LRU[i] = -1;
            OPT[i] = -1;
            c_OPT[i] = INT_MAX;
        }
        Random_instruction();
        Show();
    }
    void n_block();            //读取页块个数
    void Random_instruction(); //产生随机指令
    void _FIFO(int n);         //FIFO算法
    void _LRU(int n);          //LRU算法
    void _OPT(int *m);         //最佳页面置换算法
    void Show();               //结果展示
};

void RPS::n_block()
{
    cout << "Please input the number of page block:";
    cin >> block_n;
}

void RPS::Random_instruction()
{
    srand(time(NULL));
    int m0, m1, m2;
    int m[num];
    int i = 0;
    for (; i < num; i++)
    {
        m0 = rand() % 30 + 1;
        _FIFO(m0);
        _LRU(m0);
        m[i] = m0;
        i++;
        if (i == num - 1)
            break;
        _FIFO(m0 + 1);
        _LRU(m0 + 1);
        m[i] = m0 + 1;
        i++;
        if (i == num - 1)
            break;
        if (m0 == 1)
            m1 = 0;
        else
            m1 = rand() % (m0 - 1);
        _FIFO(m1);
        _LRU(m1);
        m[i] = m1;
        i++;
        if (i == num - 1)
            break;
        _FIFO(m1 + 1);
        _LRU(m1 + 1);
        m[i] = m1 + 1;
        i++;
        if (i == num - 1)
            break;

        m2 = rand() % 30 + (m1 + 2);
        _FIFO(m2);
        _LRU(m2);
        m[i] = m2;
        i++;
        if (i == num - 1)
            break;
        _FIFO(m2 + 1);
        _LRU(m2 + 1);
        m[i] = m2 + 1;
    }
    _OPT(m);
}

void RPS::_FIFO(int n)
{
    FIFO *p = F_first;
    if (F_block_m == 0)
    { //内存空
        p->n = n;
        F_block_m++;
        return;
    }
    while (p->n != n && p->next && p->next != F_first)
        p = p->next;
    if (p->n == n)
        return;
    if (F_block_m < block_n)
    { //内存块未满
        FIFO *q = new FIFO(n);
        p->next = q;
        p = p->next;
        F_block_m++;
    }
    else if (F_block_m == block_n && !p->next)
    { //内存恰满
        //环形链表
        p->next = F_first;
        //置换
        F_first->n = n;
        F_first = F_first->next;
        F_num++;
    }
    else
    { //内存已满
        //置换
        F_first->n = n;
        F_first = F_first->next;
        F_num++;
    }
}

void RPS::_LRU(int n)
{
    for (int i = 0; i < block_n; i++)
    {
        if (LRU[i] == -1)
        { //内存未满
            LRU[i] = n;
            L_block_m++;
            return;
        }
        if (LRU[i] == n)
        {
            for (int j = i; j < L_block_m; j++)
                LRU[j] = LRU[j + 1];
            LRU[L_block_m] = n;
            return;
        }
    }
    //置换
    for (int i = 0; i < L_block_m - 1; i++)
        LRU[i] = LRU[i + 1];
    LRU[L_block_m - 1] = n;
    L_num++;
}

void RPS::_OPT(int *m)
{
    int j = 0;
    int t, temp;
    int Max = INT_MIN;
    while (o_block_m < block_n)
        for (int i = 0; i < block_n; i++)
        {
            if (OPT[i] == -1)
            {
                OPT[i] = m[j];
                j++;
                o_block_m++;
                break;
            }
            if (OPT[i] == m[j])
            {
                j++;
                break;
            }
        }
    while (m[j])
    {
        for (int i = 0; i < block_n; i++)
            if (m[j] && OPT[i] == m[j])
            {
                j++;
                i = 0;
            }
        for (int i = 0; i < block_n; i++)
        {
            for (t = j; t < num - 1; t++)
                if (m[t] == OPT[i] && t > Max)
                {
                    temp = i;
                    Max = t;
                    break;
                }
            if (t == num - 1)
            {
                temp = i;
                break;
            }
        }
        //置换
        OPT[temp] = m[j];
        j++;
        O_num++;
    }
}

void RPS::Show()
{
    cout << "FIFO: " << 1 - (double)F_num / num << endl;
    cout << "LRU: " << 1 - (double)L_num / num << endl;
    cout << "OPT: " << 1 - (double)O_num / num << endl;
}

int main()
{
    RPS R;
    system("puase");
    return 0;
}