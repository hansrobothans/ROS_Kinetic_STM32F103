#include "Kinematics.h"

Kinematics::Kinematics(int motor_max_rpm, float wheel_diameter, float base_width, int pwm_bits)
{
  wheel_diameter_ = wheel_diameter;
  circumference_ = PI * wheel_diameter_;
  max_rpm_ = motor_max_rpm;
  base_width_ = base_width;
  pwm_res_ = pow((float)2, pwm_bits) - 1;
}

Kinematics::output Kinematics::getRPM(float linear_x, float linear_y, float angular_z)
{
  //convert m/s to m/min
  linear_vel_x_mins_ = linear_x * 60;
  linear_vel_y_mins_ = linear_y * 60;

  //convert rad/s to rad/min
  angular_vel_z_mins_ = angular_z * 60;

  //Vt = ω * radius
  tangential_vel_ = angular_vel_z_mins_ * base_width_;

  x_rpm_ = linear_vel_x_mins_ / circumference_;
  y_rpm_ = linear_vel_y_mins_ / circumference_;
  tan_rpm_ = tangential_vel_ / circumference_;

  Kinematics::output rpm;

  //calculate for the target motor RPM and direction
  //front-left motor
  rpm.motor1 = (x_rpm_ - y_rpm_ - tan_rpm_) * Left_coefficient;
  //rear-left motor
  rpm.motor3 = x_rpm_ + y_rpm_ - tan_rpm_;

  //front-right motor
  rpm.motor2 = (x_rpm_ + y_rpm_ + tan_rpm_) * Right_coefficient;
  //rear-right motor
  rpm.motor4 = x_rpm_ - y_rpm_ + tan_rpm_;

  return rpm;
}

Kinematics::output Kinematics::getPWM(float linear_x, float linear_y, float angular_z)
{
  Kinematics::output rpm;
  Kinematics::output pwm;

  rpm = getRPM(linear_x, linear_y, angular_z);

  //convert from RPM to PWM
  //front-left motor
  pwm.motor1 = rpmToPWM(rpm.motor1);
  //rear-left motor
  pwm.motor2 = rpmToPWM(rpm.motor2);

  //front-right motor
  pwm.motor3 = rpmToPWM(rpm.motor3);
  //rear-right motor
  pwm.motor4 = rpmToPWM(rpm.motor4);

  return pwm;
}

Kinematics::velocities Kinematics::getVelocities(int motor1, int motor2)
{
  Kinematics::velocities vel;

  double average_rpm_x = (motor1 + motor2) / 2; // RPM
  //convert revolutions per minute to revolutions per second
  double average_rps_x = average_rpm_x / 60; // RPS
  vel.linear_x = (average_rps_x * (wheel_diameter_ * PI)); // m/s

  double average_rpm_a = (motor2 - motor1) / 2;
  //convert revolutions per minute to revolutions per second
  double average_rps_a = average_rpm_a / 60;
  vel.angular_z =  (average_rps_a * (wheel_diameter_ * PI)) / base_width_;

  return vel;
}

int Kinematics::rpmToPWM(int rpm)
{
  //remap scale of target RPM vs MAX_RPM to PWM
 return (((double) rpm / (double) max_rpm_) * 255);
}
