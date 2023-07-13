#include <ESP32Servo.h>

Servo aircon_servo;

int motor_pin = 14;
int off_pos = 0;
int pos_1 = 50;
int pos_2 = 100;
int pos_3 = 150;

void turn_off(){
  aircon_servo.write(off_pos);
  stop_everything();
}

void turn_on_1(){
  aircon_servo.write(pos_1);
}

void turn_on_2(){
  aircon_servo.write(pos_2);
}

void turn_on_3(){
  aircon_servo.write(pos_3);
}

void init_servo(){
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  aircon_servo.setPeriodHertz(50);  
  aircon_servo.attach(motor_pin, 500, 1850);
  
  //aircon_servo.attach(13);
}
