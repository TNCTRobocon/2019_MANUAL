#include "xcon.h"

#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include "LIB/TACP/TACP.h"

XBOX_ST_t xbox_st;

static uint8_t _getc(void){
    return Serial3.read();
}
static unsigned int _available(void){
    return Serial3.available();
}
static void _putc(uint8_t temp){
    Serial3.write(temp);
}

static TACP xcon={
    .serial_getc=_getc,
    .serial_available=_available,
    .serial_putc=_putc,
    .buffer_size=32,
    .timeout=3
};

void xcon_init(){
    TACP_init(&xcon);
    Serial3.begin(115200);
}

void xcon_update(){
    static int timeout_cnt=0;
    if(TACP_update(&xcon)){
        xbox_st.LX=xcon.rcpac.data[0];
        xbox_st.LY=xcon.rcpac.data[1];
        xbox_st.RX=xcon.rcpac.data[2];
        xbox_st.RY=xcon.rcpac.data[3];
        xbox_st.L2=xcon.rcpac.data[4];
        xbox_st.R2=xcon.rcpac.data[5];
        if(xcon.rcpac.data[6]&0b10000000){
            xbox_st.LX*=-1;
        }
        if(xcon.rcpac.data[6]&0b1000000){
            xbox_st.LY*=-1;
        }
        if(xcon.rcpac.data[6]&0b100000){
            xbox_st.RX*=-1;
        }
        if(xcon.rcpac.data[6]&0b10000){
            xbox_st.RY*=-1;
        }
        xbox_st.BATTERY=xcon.rcpac.data[6]%0b10000;
        xbox_st.UP=xcon.rcpac.data[7]&0b10000000;
        xbox_st.DOWN=xcon.rcpac.data[7]&0b1000000;
        xbox_st.LEFT=xcon.rcpac.data[7]&0b100000;
        xbox_st.RIGHT=xcon.rcpac.data[7]&0b10000;
        xbox_st.A=xcon.rcpac.data[7]&0b1000;
        xbox_st.B=xcon.rcpac.data[7]&0b100;
        xbox_st.X=xcon.rcpac.data[7]&0b10;
        xbox_st.Y=xcon.rcpac.data[7]&0b1;
        xbox_st.L1=xcon.rcpac.data[8]&0b10000000;
        xbox_st.R1=xcon.rcpac.data[8]&0b1000000;
        xbox_st.L3=xcon.rcpac.data[8]&0b100000;
        xbox_st.R3=xcon.rcpac.data[8]&0b10000;
        xbox_st.START=xcon.rcpac.data[8]&0b1000;
        xbox_st.BACK=xcon.rcpac.data[8]&0b100;
        xbox_st.XBOX=xcon.rcpac.data[8]&0b10;
        xbox_st.CONNECTING=xcon.rcpac.data[8]&0b1;
        timeout_cnt=0;
    }else{
        timeout_cnt++;
        if(timeout_cnt>=1000){
            xbox_st.CONNECTING=false;
            timeout_cnt=0;
        }
    }
}