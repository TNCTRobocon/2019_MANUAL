#ifndef MOTOR_H_
#define MOTOR_H_

void motor_update();
void motor_set_RPM(int num,double RPM);
void motor_set_DT(int num,double DT);
int motor_get_output(int num);

#endif