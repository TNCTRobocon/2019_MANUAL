#include "encoder.h"

#include <Arduino.h>

#define ENCODER_NUM 8

#define NOMAL 1
#define REVERSE -1

typedef struct Encoder{
    int pin_A;
    int pin_B;
    int direction;
    int last_sig;
    int pulse;
    double RPM;
}Encoder_t;

Encoder_t enc[ENCODER_NUM]={
    {25,23,NOMAL},
    {29,27,NOMAL},
    {33,31,NOMAL},
    {37,35,NOMAL},
    {41,39,NOMAL},
    {45,43,NOMAL},
    {49,47,NOMAL},
    {53,51,NOMAL}
};

void encoder_init(){
    for(int i=0;i<ENCODER_NUM;i++){
        pinMode(enc[i].pin_A,INPUT);
        pinMode(enc[i].pin_B,INPUT);
        enc[i].last_sig=enc[i].pulse=enc[i].RPM=0;
    }
}

//500us周期
void encoder_update(){
    static int cnt_time;

    int sig;

    for(int i=0;i<ENCODER_NUM;i++){
        sig=(digitalRead(enc[i].pin_A)<<1)|(digitalRead(enc[i].pin_A)^digitalRead(enc[i].pin_B));
        switch(sig-enc[i].last_sig){
        case 1:
        case -3:
            enc[i].pulse++;
            break;
        case -1:
        case 3:
            enc[i].pulse--;
            break;
        }
        enc[i].last_sig=sig;
    }

    cnt_time++;
    if(cnt_time>=100){
        for(int i=0;i<ENCODER_NUM;i++){
            enc[i].RPM=enc[i].direction*((enc[i].pulse/0.05)/400)*60;
            enc[i].pulse=0;
        }
        cnt_time=0;
    }
}



double encoder_get_RPM(int num){
    return enc[num].RPM;
}