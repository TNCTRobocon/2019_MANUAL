/*
3 /         \ 0
 /<         <\    ^+
                ---0[rad]
 \>         >/
2 \         / 1
 */
#ifndef OMNI4_H_
#define OMNI4_H_

#include "../PID/pid.h"

#ifdef NEED_NEW_H
#include <new.h>
#endif

class Omni4{
private:
    class Position{
    public:
        double x_velocity,y_velocity,angular_velocity;
        double x,y,angle;
        Position(){
            x_velocity=y_velocity=angular_velocity=x=y=angle=0;
        }
    };

    const double _distance_from_center;//[m]
    const double _diameter_of_wheel;//[m]
    const double _angle_offset;//[rad]

    bool called_feedback_init;
    bool feedback_mode;
    bool auto_mode;

    double feedback_interval;

    PID *pid_x;
    PID *pid_y;
    PID *pid_angle;

    Position current_position;
    Position target_position;
    Position virtual_point_for_auto;
    Position target_virtual_point_for_auto;
    double target_RPM[4];

    void auto_move_update();
public:

    Omni4(double distance_from_center,double diameter_of_wheel,double angle_offset):_distance_from_center(distance_from_center),_diameter_of_wheel(diameter_of_wheel),_angle_offset(angle_offset)
    {
        for(int i=0;i<4;i++){
            target_RPM[i]=0;
        }
        called_feedback_init=feedback_mode=auto_mode=false;
    }
    ~Omni4(){
        delete pid_x;
        delete pid_y;
        delete pid_angle;
    }
    void feedback_init(double interval,double max_velocity,double max_angular_velocity,double Kp_xy,double Ki_xy,double Kd_xy,double Kp_angle,double Ki_angle,double Kd_angle);
    int configuration_feedback_mode(bool mode);
    int configuration_auto_mode(bool mode);

    void calculate_RPM_relative(double velocity,double angle,double angular_velocity);
    void calculate_RPM_absolute(double velocity,double angle,double angular_velocity);

    void calculate_position(double time,double RPM_0,double RPM_1,double RPM_2,double RPM_3);
    void set_target_position(double x,double y,double angle);
    void feedback_position();

    void auto_move_start(double x,double y,double angle,double velocity,double angular_velocity);
    void auto_move_stop();

    double get_target_RPM(int num);
    double get_current_x();
    double get_current_y();
    double get_current_angle();
    double get_current_x_velocity();
    double get_current_y_velocity();
    double get_current_angular_velocity();
};

#endif