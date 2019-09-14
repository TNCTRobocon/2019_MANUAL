#include "motor.h"

#include <Arduino.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../LIB/PID/pid.h"
#include "encoder.h"

#define NOMAL 1
#define REVERSE -1

#define MOTOR_NUM 8

#define Kp 0.1
#define Ki 0
#define Kd 0.01

class Motor{
private:
    const int _pin_A;
    const int _pin_B;
    const int _pin_PWM;
    const int _enc_num;
    const int _direction;
    bool RPM_or_DT;//true:RPM
    double setpoint;
    int output;
    PID pid;
public:
    Motor(int pin_A,int pin_B,int pin_PWM,int enc_num,int direction):_pin_A(pin_A),_pin_B(pin_B),_pin_PWM(pin_PWM),_enc_num(enc_num),_direction(direction),pid(0.05,255,-255,Kp,Ki,Kd){
        pinMode(_pin_A,OUTPUT);
        pinMode(_pin_B,OUTPUT);
        pinMode(_pin_PWM,OUTPUT);
        digitalWrite(_pin_A,LOW);
        digitalWrite(_pin_B,LOW);
        analogWrite(_pin_PWM,0);
        RPM_or_DT=false;
        output=0;
    }
    void update(){
        double pv;
        if(RPM_or_DT){
            pv=encoder_get_RPM(_enc_num);
            output+=pid.calculate(setpoint,pv);
            if(output>255){
                output=255;
            }else if(output<-255){
                output=-255;
            }
            analogWrite(_pin_PWM,abs(output));
            if(_direction*output>0){
                digitalWrite(_pin_A,HIGH);
                digitalWrite(_pin_B,LOW);
            }else if(_direction*output<0){
                digitalWrite(_pin_A,LOW);
                digitalWrite(_pin_B,HIGH);
            }else{
                digitalWrite(_pin_A,LOW);
                digitalWrite(_pin_B,LOW);
            }
        }
    }
    void set_RPM(double RPM){
        setpoint=RPM;
        RPM_or_DT=true;
    }
    void set_DT(double DT){
        output=255*DT;
        if(output>255){
            output=255;
        }else if(output<-255){
            output=-255;
        }
        analogWrite(_pin_PWM,abs(output));
        if(_direction*output>0){
            digitalWrite(_pin_A,HIGH);
            digitalWrite(_pin_B,LOW);
        }else if(_direction*output<0){
            digitalWrite(_pin_A,LOW);
            digitalWrite(_pin_B,HIGH);
        }else{
            digitalWrite(_pin_A,LOW);
            digitalWrite(_pin_B,LOW);
        }
        RPM_or_DT=false;
    }
    int get_output(){
        return output;
    }
};

Motor motor[MOTOR_NUM]={
    Motor(24,22,2,0,REVERSE),
    Motor(28,26,3,1,REVERSE),
    Motor(32,30,5,2,NOMAL),
    Motor(36,34,6,3,REVERSE),
    Motor(40,38,7,4,NOMAL),
    Motor(44,42,10,5,NOMAL),
    Motor(48,46,11,6,NOMAL),
    Motor(52,50,12,7,NOMAL)
};

void motor_update(){
    for(int i=0;i<MOTOR_NUM;i++){
        motor[i].update();
    }
}

void motor_set_RPM(int num,double RPM){
    motor[num].set_RPM(RPM);
}

void motor_set_DT(int num,double DT){
    motor[num].set_DT(DT);
}

int motor_get_output(int num){
    return motor[num].get_output();
}