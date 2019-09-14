#include "sm_arm.h"

#include <Arduino.h>

#include "../LIB/STMACHINE/statemachine.h"
#include "../SLV/slv.h"
#include "../SW/sw.h"
#include "../XCON/xcon.h"
#include "../MOTOR/motor.h"
#include "basket.h"

#define ARM_SPEED 0.13

static bool flag_cylinder=false;
static bool flag_hand=true;

static bool get_click_X(){
    static bool flag=false;
    if(xbox_st.X){
        if(!flag){
            flag=true;
            return true;
        }
    }else{
        flag=false;
    }
    return false;
}

static bool get_click_Y(){
    static bool flag=false;
    if(xbox_st.Y){
        if(!flag){
            flag=true;
            return true;
        }
    }else{
        flag=false;
    }
    return false;
}

static void switch_hand(){
    if(flag_hand){
        SLV_open(4);
        flag_hand=false;
    }else{
        SLV_close(4);
        flag_hand=true;
    }
}

static void switch_cylinder(){
    if(flag_cylinder){
        SLV_close(3);
        flag_cylinder=false;
    }else{
        SLV_open(3);
        flag_cylinder=true;
    }
}

char* get_condition_arm(){
    if(SW_read_2()){
        return "leftlimit";
    }else if(SW_read_1()){
        return "rightlimit";
    }else{
        return "neutral";
    }
}

class RightLimit:public StateBase{
public:
    void entry(void* context){}
    void update(void* context){
        if(xbox_st.L1&&!flag_cylinder){
            motor_set_DT(4,-ARM_SPEED);
        }else{
            motor_set_DT(4,0);
        }
        if(get_click_X()){
            switch_cylinder();
        }
        if(get_click_Y()){
            switch_hand();
        }
        basket();
    }
    void exit(void* context){}
};

class LeftLimit:public StateBase{
public:
    void entry(void* context){}
    void update(void* context){
        if(xbox_st.R1){
            motor_set_DT(4,ARM_SPEED);
        }else{
            motor_set_DT(4,0);
        }
        if(get_click_Y()){
            switch_hand();
        }
    }
    void exit(void* context){}
};

class Neutral:public StateBase{
public:
    void entry(void* context){}
    void update(void* context){
        if(xbox_st.L1&&!flag_cylinder){
            motor_set_DT(4,-ARM_SPEED);
        }else if(xbox_st.R1){
            motor_set_DT(4,ARM_SPEED);
        }else{
            motor_set_DT(4,0);
        }
        if(get_click_Y()){
            switch_hand();
        }
    }
    void exit(void* context){}
};

StateMachine sm_arm("neutral_state");
TransList list_rightlimit;
TransList list_leftlimit;
TransList list_neutral;
StateBase* rightlimit_state=new RightLimit;
StateBase* leftlimit_state=new LeftLimit;
StateBase* neutral_state=new Neutral;

void sm_arm_init(){
    list_rightlimit.add("neutral","neutral_state");
    list_rightlimit.add("leftlimit","leftlimit_state");
    list_leftlimit.add("neutral","neutral_state");
    list_leftlimit.add("rightlimit","rightlimit_state");
    list_neutral.add("leftlimit","leftlimit_state");
    list_neutral.add("rightlimit","rightlimit_state");
    sm_arm.add("rightlimit_state",rightlimit_state,&list_rightlimit,get_condition_arm);
    sm_arm.add("leftlimit_state",leftlimit_state,&list_leftlimit,get_condition_arm);
    sm_arm.add("neutral_state",neutral_state,&list_neutral,get_condition_arm);
}

void sm_arm_run(){
    sm_arm.run(NULL);
}