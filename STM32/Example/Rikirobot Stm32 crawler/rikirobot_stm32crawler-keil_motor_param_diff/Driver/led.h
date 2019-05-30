#ifndef _LED_H_
#define _LED_H_

#include "config.h"

class Led {
public:
	void init();
	void on_off(bool status);
};

#endif //_LED_H_
