#include "omni4.h"

#include <math.h>

static inline void simplification_angle(double* angle){
    if(*angle>M_PI){
        *angle-=2*M_PI;
    }else if(*angle<-M_PI){
        *angle+=2*M_PI;
    }
}

void Omni4::feedback_init(double interval,double max_velocity,double max_angular_velocity,double Kp_xy,double Ki_xy,double Kd_xy,double Kp_angle,double Ki_angle,double Kd_angle){
    feedback_interval=interval;
    pid_x=new PID(feedback_interval,max_velocity,-max_velocity,Kp_xy,Ki_xy,Kd_xy);
    pid_y=new PID(feedback_interval,max_velocity,-max_velocity,Kp_xy,Ki_xy,Kd_xy);
    pid_angle=new PID(feedback_interval,max_angular_velocity,-max_angular_velocity,Kp_angle,Ki_angle,Kd_angle);
    called_feedback_init=true;
}

int Omni4::configuration_feedback_mode(bool mode){
    if(!called_feedback_init){
        return -1;
    }
    feedback_mode=mode;
    return 0;
}

int Omni4::configuration_auto_mode(bool mode){
    if(!called_feedback_init){
        return -1;
    }
    feedback_mode=auto_mode=mode;
    return 0;
}

void Omni4::calculate_RPM_relative(double velocity,double angle,double angular_velocity){
    static const double k1=60/(M_PI*_diameter_of_wheel);
    static const double k2=_distance_from_center*k1;
    target_RPM[0]=k1*velocity*sin(angle-M_PI/4-_angle_offset);
    target_RPM[1]=k1*velocity*sin(angle+M_PI/4-_angle_offset);
    target_RPM[2]=-k1*velocity*sin(angle-M_PI/4-_angle_offset);
    target_RPM[3]=-k1*velocity*sin(angle+M_PI/4-_angle_offset);
    for(int i=0;i<4;i++){
        target_RPM[i]+=k2*angular_velocity;
    }
}

void Omni4::calculate_RPM_absolute(double velocity,double angle,double angular_velocity){
    static const double k1=60/(M_PI*_diameter_of_wheel);
    static const double k2=_distance_from_center*k1;
    double offset=current_position.angle+_angle_offset;
    target_RPM[0]=k1*velocity*sin(angle-M_PI/4-offset);
    target_RPM[1]=k1*velocity*sin(angle+M_PI/4-offset);
    target_RPM[2]=-k1*velocity*sin(angle-M_PI/4-offset);
    target_RPM[3]=-k1*velocity*sin(angle+M_PI/4-offset);
    for(int i=0;i<4;i++){
        target_RPM[i]+=k2*angular_velocity;
    }
}

void Omni4::calculate_position(double time,double RPM_0,double RPM_1,double RPM_2,double RPM_3){
    double motor_velocity[4];
    motor_velocity[0]=(RPM_0/60)*(M_PI*_diameter_of_wheel);
    motor_velocity[1]=(RPM_1/60)*(M_PI*_diameter_of_wheel);
    motor_velocity[2]=(RPM_2/60)*(M_PI*_diameter_of_wheel);
    motor_velocity[3]=(RPM_3/60)*(M_PI*_diameter_of_wheel);

    current_position.angular_velocity=((motor_velocity[0]+motor_velocity[1]+motor_velocity[2]+motor_velocity[3])/4)/_distance_from_center;
    current_position.angle+=current_position.angular_velocity*time;
    simplification_angle(&current_position.angle);

    for(int i=0;i<4;i++){
        motor_velocity[i]-=current_position.angular_velocity*_distance_from_center;
    }

    double offset=_angle_offset+(current_position.angle-current_position.angular_velocity*time/2);
    current_position.x_velocity=(motor_velocity[0]*cos(3*M_PI/4+offset)+motor_velocity[1]*cos(M_PI/4+offset)+motor_velocity[2]*cos(-M_PI/4+offset)+motor_velocity[3]*cos(-3*M_PI/4+offset))/2;
    current_position.y_velocity=(motor_velocity[0]*sin(3*M_PI/4+offset)+motor_velocity[1]*sin(M_PI/4+offset)+motor_velocity[2]*sin(-M_PI/4+offset)+motor_velocity[3]*sin(-3*M_PI/4+offset))/2;
    current_position.x+=current_position.x_velocity*time;
    current_position.y+=current_position.y_velocity*time;
}

void Omni4::set_target_position(double x,double y,double angle){
    target_position.x=x;
    target_position.y=y;
    simplification_angle(&angle);
    target_position.angle=angle;
}

void Omni4::feedback_position(){
    if(!feedback_mode){
        return;
    }

    if(auto_mode){
        auto_move_update();
    }

    target_position.x_velocity=pid_x->calculate(target_position.x,current_position.x);
    target_position.y_velocity=pid_y->calculate(target_position.y,current_position.y);
    double deviation_angle=target_position.angle-current_position.angle;
    simplification_angle(&deviation_angle);
    target_position.angular_velocity=pid_angle->calculate(target_position.angle,target_position.angle-deviation_angle);

    double velocity=sqrt(pow(target_position.x_velocity,2)+pow(target_position.y_velocity,2));
    double angle;
    if(velocity==0){
        angle=0;
    }else{
        angle=atan(target_position.y_velocity/fabs(target_position.x_velocity));
        if(target_position.x_velocity<0){
            if(angle>0){
                angle=M_PI-angle;
            }else if(angle<0){
                angle=-M_PI-angle;
            }
        }
    }
    double angular_velocity=target_position.angular_velocity;
    calculate_RPM_absolute(velocity,angle,angular_velocity);
}

void Omni4::auto_move_start(double x,double y,double angle,double velocity,double angular_velocity){
    target_virtual_point_for_auto.x=x;
    target_virtual_point_for_auto.y=y;
    simplification_angle(&angle);
    target_virtual_point_for_auto.angle=angle;

    double temp_angle;
    if((target_virtual_point_for_auto.x-virtual_point_for_auto.x)==0&&(target_virtual_point_for_auto.y-virtual_point_for_auto.y)==0){
        temp_angle=0;
    }else{
        temp_angle=atan((target_virtual_point_for_auto.y-virtual_point_for_auto.y)/fabs(target_virtual_point_for_auto.x-virtual_point_for_auto.x));
        if((target_virtual_point_for_auto.x-virtual_point_for_auto.x)<0){
            if(temp_angle>0){
                temp_angle=M_PI-temp_angle;
            }else if(angle<0){
                temp_angle=-M_PI-temp_angle;
            }
        }
    }
    virtual_point_for_auto.x_velocity=velocity*cos(temp_angle);
    virtual_point_for_auto.y_velocity=velocity*sin(temp_angle);
    double deviation_angle=target_virtual_point_for_auto.angle-virtual_point_for_auto.angle;
    simplification_angle(&deviation_angle);
    if(deviation_angle>0){
        virtual_point_for_auto.angular_velocity=angular_velocity;
    }else{
        virtual_point_for_auto.angular_velocity=-angular_velocity;
    }
}

void Omni4::auto_move_stop(){
    virtual_point_for_auto.x=current_position.x;
    virtual_point_for_auto.y=current_position.y;
    virtual_point_for_auto.angle=current_position.angle;
    virtual_point_for_auto.x_velocity=virtual_point_for_auto.y_velocity=virtual_point_for_auto.angular_velocity=0;
    set_target_position(virtual_point_for_auto.x,virtual_point_for_auto.y,virtual_point_for_auto.angle);
}

void Omni4::auto_move_update(){
    virtual_point_for_auto.x+=virtual_point_for_auto.x_velocity*feedback_interval;
    virtual_point_for_auto.y+=virtual_point_for_auto.y_velocity*feedback_interval;
    virtual_point_for_auto.angle+=virtual_point_for_auto.angular_velocity*feedback_interval;
    simplification_angle(&virtual_point_for_auto.angle);
    if(target_virtual_point_for_auto.angle>0&&virtual_point_for_auto.angle<0){
        virtual_point_for_auto.angle+=2*M_PI;
    }else if(target_virtual_point_for_auto.angle<0&&virtual_point_for_auto.angle>0){
        virtual_point_for_auto.angle-=2*M_PI;
    }

    if(virtual_point_for_auto.x_velocity>0&&virtual_point_for_auto.x>=target_virtual_point_for_auto.x){
        virtual_point_for_auto.x=target_virtual_point_for_auto.x;
        virtual_point_for_auto.x_velocity=0;
    }else if(virtual_point_for_auto.x_velocity<0&&virtual_point_for_auto.x<=target_virtual_point_for_auto.x){
        virtual_point_for_auto.x=target_virtual_point_for_auto.x;
        virtual_point_for_auto.x_velocity=0;
    }
    if(virtual_point_for_auto.y_velocity>0&&virtual_point_for_auto.y>=target_virtual_point_for_auto.y){
        virtual_point_for_auto.y=target_virtual_point_for_auto.y;
        virtual_point_for_auto.y_velocity=0;
    }else if(virtual_point_for_auto.y_velocity<0&&virtual_point_for_auto.y<=target_virtual_point_for_auto.y){
        virtual_point_for_auto.y=target_virtual_point_for_auto.y;
        virtual_point_for_auto.y_velocity=0;
    }
    if(virtual_point_for_auto.angular_velocity>0&&virtual_point_for_auto.angle>=target_virtual_point_for_auto.angle){
        virtual_point_for_auto.angle=target_virtual_point_for_auto.angle;
        virtual_point_for_auto.angular_velocity=0;
    }else if(virtual_point_for_auto.angular_velocity<0&&virtual_point_for_auto.angle<=target_virtual_point_for_auto.angle){
        virtual_point_for_auto.angle=target_virtual_point_for_auto.angle;
        virtual_point_for_auto.angular_velocity=0;
    }

    set_target_position(virtual_point_for_auto.x,virtual_point_for_auto.y,virtual_point_for_auto.angle);
}

double Omni4::get_target_RPM(int num){
    if(num<0||num>3){
        return 0;
    }
    return target_RPM[num];
}

double Omni4::get_current_x(){
    return current_position.x;
}

double Omni4::get_current_y(){
    return current_position.y;
}

double Omni4::get_current_angle(){
    return current_position.angle;
}

double Omni4::get_current_x_velocity(){
    return current_position.x_velocity;
}

double Omni4::get_current_y_velocity(){
    return current_position.y_velocity;
}

double Omni4::get_current_angular_velocity(){
    return current_position.angular_velocity;
}