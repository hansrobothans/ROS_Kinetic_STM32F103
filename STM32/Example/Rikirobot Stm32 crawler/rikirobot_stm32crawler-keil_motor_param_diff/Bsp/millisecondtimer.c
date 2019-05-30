#ifdef __cplusplus
extern "C" {
#endif

#include "millisecondtimer.h"

volatile uint32_t _counter;

void initialise(void) 
{
	_counter = 0;
	SysTick_Config(SystemCoreClock / 1000);
}

void delay(uint32_t millis) 
{
	uint32_t target;

	target = _counter + millis;
	while(_counter < target);
} 

void SysTick_Handler(void) 
{
	_counter++;
}

uint32_t millis(void) 
{
	return _counter;
}

void reset(void) 
{
	_counter = 0;
}

#ifdef __cplusplus
}
#endif

