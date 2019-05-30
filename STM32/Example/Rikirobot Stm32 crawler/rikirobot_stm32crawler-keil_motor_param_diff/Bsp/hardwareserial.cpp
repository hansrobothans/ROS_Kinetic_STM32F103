#include "hardwareserial.h"
#include "interrupt.h"

USART_TypeDef*  SERIAL_USART[SERIALn] = {RIKI_SERIAL1, RIKI_SERIAL2, RIKI_SERIAL3};
GPIO_TypeDef*   SERIAL_PORT[SERIALn] = {RIKI_SERIAL1_GPIO_PORT, RIKI_SERIAL2_GPIO_PORT, RIKI_SERIAL3_GPIO_PORT}; 
const uint32_t  SERIAL_USART_CLK[SERIALn] = {RIKI_SERIAL1_CLK, RIKI_SERIAL2_CLK, RIKI_SERIAL3_CLK};
const uint32_t  SERIAL_PORT_CLK[SERIALn] = {RIKI_SERIAL1_GPIO_CLK, RIKI_SERIAL2_GPIO_CLK, RIKI_SERIAL3_GPIO_CLK};
const uint16_t  SERIAL_TX_PIN[SERIALn] = {RIKI_SERIAL1_TX_PIN, RIKI_SERIAL2_TX_PIN, RIKI_SERIAL3_TX_PIN}; 
const uint16_t  SERIAL_RX_PIN[SERIALn] = {RIKI_SERIAL1_RX_PIN, RIKI_SERIAL2_RX_PIN, RIKI_SERIAL2_RX_PIN}; 
const uint16_t  SERIAL_IRQn[SERIALn] = {RIKI_SERIAL1_IRQ, RIKI_SERIAL2_IRQ, RIKI_SERIAL3_IRQ};
const uint16_t  SERILA_NVIC[SERIALn] = {RIKI_SERIAL1_NVIC, RIKI_SERIAL2_NVIC, RIKI_SERIAL3_NVIC};
         

HardwareSerial::HardwareSerial(Serial_TypeDef _Serial)
{
		Serial = _Serial;

		if(this->Serial == SERIAL1)
			Serial1 = this;
		if(this->Serial == SERIAL2)
			Serial2 = this;
		if(this->Serial == SERIAL3)
			Serial3 = this;
}


void HardwareSerial::begin(uint32_t baud)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
  
	if(this->Serial == SERIAL1) {
		RCC_APB2PeriphClockCmd(SERIAL_USART_CLK[this->Serial]|SERIAL_PORT_CLK[this->Serial], ENABLE);	
	} else {
		RCC_APB1PeriphClockCmd(SERIAL_USART_CLK[this->Serial], ENABLE); 
		RCC_APB2PeriphClockCmd(SERIAL_PORT_CLK[this->Serial], ENABLE); 
	}
	//USART_TX   
	GPIO_InitStructure.GPIO_Pin = SERIAL_TX_PIN[this->Serial];        	 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	 	 
	GPIO_Init(SERIAL_PORT[this->Serial], &GPIO_InitStructure);

	//USART_RX	 
	GPIO_InitStructure.GPIO_Pin = SERIAL_RX_PIN[this->Serial];           
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(SERIAL_PORT[this->Serial], &GPIO_InitStructure);  

	USART_InitStructure.USART_BaudRate	 = baud;				
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	
	USART_InitStructure.USART_StopBits 	 = USART_StopBits_1;	
	USART_InitStructure.USART_Parity	 = USART_Parity_No;		
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

	USART_Init(SERIAL_USART[this->Serial], &USART_InitStructure); 				

	NVIC_InitStructure.NVIC_IRQChannel = SERIAL_IRQn[this->Serial];       
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = SERILA_NVIC[this->Serial];		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);						   
	USART_ITConfig(SERIAL_USART[this->Serial], USART_IT_RXNE, ENABLE);

	USART_Cmd(SERIAL_USART[this->Serial], ENABLE);
}

uint32_t HardwareSerial::available(void)
{
	return (uint32_t)(SERIAL_BUFFER_SIZE + rx_buffer._iHead - rx_buffer._iTail) % SERIAL_BUFFER_SIZE ;
}


uint8_t HardwareSerial::read(void)
{
// if the head isn't ahead of the tail, we don't have any characters
  if ( rx_buffer._iHead == rx_buffer._iTail )
    return -1 ;

  uint8_t uc = rx_buffer._aucBuffer[rx_buffer._iTail] ;
  rx_buffer._iTail = (unsigned int)(rx_buffer._iTail + 1) % SERIAL_BUFFER_SIZE ;
  return uc ;

}

uint32_t HardwareSerial::write(uint8_t ch)
{
	USART_SendData(SERIAL_USART[this->Serial], ch);
	while(USART_GetFlagStatus(SERIAL_USART[this->Serial], USART_FLAG_TXE) == RESET); 
	return 1; 
}

void HardwareSerial::flush()
{
	rx_buffer._iTail = rx_buffer._iHead;
}

void HardwareSerial::print(const char *format, ...)
{
	va_list args;
	char buf[256];
	va_start (args, format);
	vsprintf (buf, format, args);       
	va_end (args);    
	putstr(buf);

}

void HardwareSerial::putstr(const char *str)
{
	int i;
	for(i = 0; i < strlen(str); i++){	
		write(str[i]);
	}
}

void HardwareSerial::irq()
{
	uint8_t data;
	if (USART_GetITStatus(SERIAL_USART[Serial], USART_IT_RXNE) != RESET) {
		data = USART_ReceiveData(SERIAL_USART[Serial]);
		rx_buffer.store_char(data) ;  
		USART_ClearITPendingBit(SERIAL_USART[Serial], USART_IT_RXNE);    
	}
}

