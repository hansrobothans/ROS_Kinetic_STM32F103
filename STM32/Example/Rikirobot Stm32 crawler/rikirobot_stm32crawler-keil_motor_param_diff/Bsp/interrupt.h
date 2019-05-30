#ifndef _INTERRUPT_H_
#define	_INTERRUPT_H_

#include "config.h"
#include "hardwareserial.h"

#ifdef  USE_SERIAL1
extern HardwareSerial *Serial1;
#endif  

#ifdef  USE_SERIAL2
extern HardwareSerial *Serial2;
#endif 

#ifdef  USE_SERIAL3
extern HardwareSerial *Serial3;
#endif

#ifdef __cplusplus
extern "C"{
#endif
	
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);	
	
#ifdef __cplusplus
}
#endif

#endif
