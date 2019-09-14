#include "manual.h"

#include <Arduino.h>

#include "../XCON/xcon.h"
#include "../MOTOR/motor.h"
#include "move.h"
#include "sm_arm.h"

void manual_init(){
    sm_arm_init();
}

void manual_run(){
    if((!xbox_st.CONNECTING)||xbox_st.XBOX){
        for(int i=0;i<8;i++){
            motor_set_DT(i,0);
        }
        return;
    }
    move();
    sm_arm_run();
}
