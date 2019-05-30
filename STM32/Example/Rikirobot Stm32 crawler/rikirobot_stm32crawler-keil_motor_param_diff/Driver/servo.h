#ifndef _SERVO_H_
#define _SERVO_H_

#include "config.h"

class Servo {
public:
	Servo(Servo_TypeDef _servo);
	void init();
	void pos(uint32_t angle);
private:
	Servo_TypeDef servo;
};

#endif //_SERVO_H_
