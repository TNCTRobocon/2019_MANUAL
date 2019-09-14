#include "move.h"

#include <math.h>
#include <stdlib.h>

#include "../XCON/xcon.h"
#include "../MOTOR/motor.h"
#include "../CAR/car.h"

#define CONVERT_TO_DT 0.0007
#define ADJUST_ANGV 1.6

static bool get_click_START(){
    static bool flag=false;
    if(xbox_st.START){
        if(!flag){
            flag=true;
            return true;
        }
    }else{
        flag=false;
    }
    return false;
}

static inline bool RPM_or_DT(){
    static bool flag=true;
    if(get_click_START()){
        if(flag){
            flag=false;
            return false;
        }else{
            flag=true;
            return true;
        }
    }
    return flag;
}

void move(){
    if(xbox_st.BACK){
        for(int i=0;i<4;i++){
            motor_set_DT(i,0);
        }
    }

    double max_speed;
    if(xbox_st.A){
        max_speed=0.3;
    }else if(xbox_st.B){
        max_speed=1.0;
    }else{
        max_speed=0.6;;
    }
    double speed=max_speed*sqrt(pow(xbox_st.LX,2)+pow(xbox_st.LY,2))/255.0;

    double angular_velocity=((xbox_st.L2/255.0)*max_speed-(xbox_st.R2/255.0)*max_speed)*ADJUST_ANGV;

    double angle;
    if(xbox_st.LY==0&&xbox_st.LX==0){
        angle=0;
    }else{
        angle=atan(1.0*xbox_st.LY/abs(xbox_st.LX));
        if(xbox_st.LX<0){
            if(angle>0){
                angle=M_PI-angle;
            }else{
                angle=-M_PI-angle;
            }
        }
    }

    car.calculate_RPM_relative(speed,angle,angular_velocity);

    if(RPM_or_DT()){
        for(int i=0;i<4;i++){
            motor_set_RPM(i,car.get_target_RPM(i));
        }
    }else{
        for(int i=0;i<4;i++){
            motor_set_DT(i,car.get_target_RPM(i)*CONVERT_TO_DT);
        }
    }
}
