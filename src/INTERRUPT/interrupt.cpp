#include "interrupt.h"

#include <Arduino.h>

#include "../MOTOR/motor.h"
#include "../MOTOR/encoder.h"
#include "../CAR/car.h"
#include "../XCON/xcon.h"
#include "../OPERATION/manual.h"


//500us
void interrupt_service(){
    encoder_update();

    static int cnt_xcon,cnt_manual,cnt_motor,cnt_hb;
    cnt_xcon++;
    cnt_manual++;
    cnt_motor++;
    cnt_hb++;
    //1ms
    if(cnt_xcon>=2){
        xcon_update();
        cnt_xcon=0;
    }
    //10ms
    if(cnt_manual>=20){
        manual_run();
        cnt_manual=0;
    }
    //50ms
    if(cnt_motor>=100){
        car.calculate_position(0.05,encoder_get_RPM(0),encoder_get_RPM(1),encoder_get_RPM(2),encoder_get_RPM(3));
        motor_update();
        cnt_motor=0;
    }
    //500ms
    if(cnt_hb>=1000){
        digitalWrite(13,!digitalRead(13));
        cnt_hb=0;
    }
}