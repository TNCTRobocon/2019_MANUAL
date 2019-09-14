#include "basket.h"

#include "../MOTOR/motor.h"
#include "../SW/sw.h"
#include "../XCON/xcon.h"

#define BASKET_SPEED 0.3

void basket(){
    if(xbox_st.UP&&!SW_read_3()){
        motor_set_DT(6,BASKET_SPEED);
    }else if(xbox_st.DOWN){
        motor_set_DT(6,-BASKET_SPEED);
    }else{
        motor_set_DT(6,0);
    }
}