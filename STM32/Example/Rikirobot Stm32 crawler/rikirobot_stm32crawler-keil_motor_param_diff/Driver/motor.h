#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "config.h"

#define constrain(amt,low,high) \
	((amt)<(low)?(low):((amt)>(high)?(high):(amt)))


class Motor {
	public:
		int rpm;
		static int counts_per_rev_;
		Motor(Motor_TypeDef _motor, uint32_t _arr, uint32_t _psc);
		void updateSpeed(long encoder_ticks);
		void init();
		void spin(int pwm);

	private:
		Motor_TypeDef motor;
		uint32_t arr;
		uint32_t psc;

		long prev_encoder_ticks_; 
	 	unsigned long prev_update_time_; 

		void motor_pwm_init(); 
};

#endif //_MOTOR_H_
