#include "sw.h"

#include <Arduino.h>

#define SW1 A11
#define SW2 A10
#define SW3 A12

void SW_init(){
    pinMode(SW1,INPUT_PULLUP);
    pinMode(SW2,INPUT_PULLUP);
    pinMode(SW3,INPUT_PULLUP);
}

bool SW_read_1(){
    return !digitalRead(SW1);
}

bool SW_read_2(){
    return !digitalRead(SW2);
}

bool SW_read_3(){
    return !digitalRead(SW3);
}