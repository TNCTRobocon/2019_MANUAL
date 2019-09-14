#include "slv.h"

#include <Arduino.h>

#define NOMAL true
#define REVERSE false

#define SLV_NUM 5

typedef struct SLV{
    int pin_A;
    int pin_B;
    bool direction;
}SLV_t;

SLV_t slv[SLV_NUM]={
    {54,55,NOMAL},
    {56,57,NOMAL},
    {58,59,NOMAL},
    {60,61,NOMAL},
    {62,63,NOMAL}
};

void SLV_init(){
    for(int i=0;i<SLV_NUM;i++){
        pinMode(slv[i].pin_A,OUTPUT);
        pinMode(slv[i].pin_B,OUTPUT);
        SLV_close(i);
    }
}

void SLV_open(int num){
    if(slv[num].direction){
        digitalWrite(slv[num].pin_A,HIGH);
        digitalWrite(slv[num].pin_B,LOW);
    }else{
        digitalWrite(slv[num].pin_A,LOW);
        digitalWrite(slv[num].pin_B,HIGH);
    }
}

void SLV_close(int num){
    if(!slv[num].direction){
        digitalWrite(slv[num].pin_A,HIGH);
        digitalWrite(slv[num].pin_B,LOW);
    }else{
        digitalWrite(slv[num].pin_A,LOW);
        digitalWrite(slv[num].pin_B,HIGH);
    }
}