#ifndef XCON_H_
#define XCON_H_

typedef struct XBOX_ST{
    int L2;
    int R2;
    int LX;
    int LY;
    int RX;
    int RY;
    bool UP;
    bool DOWN;
    bool LEFT;
    bool RIGHT;
    bool L1;
    bool R1;
    bool L3;
    bool R3;
    bool A;
    bool B;
    bool X;
    bool Y;
    bool START;
    bool BACK;
    bool XBOX;
    int BATTERY;
    bool CONNECTING;
}XBOX_ST_t;

extern XBOX_ST_t xbox_st;

void xcon_init();
void xcon_update();

#endif