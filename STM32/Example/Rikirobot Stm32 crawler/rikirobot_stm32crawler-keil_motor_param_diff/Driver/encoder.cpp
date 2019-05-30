#include "encoder.h"

GPIO_TypeDef*  ENCODER_PORT[ENCODERn] = {RIKI_ENCODER1_GPIO_PORT, RIKI_ENCODER2_GPIO_PORT};
TIM_TypeDef*   ENCODER_TIM[ENCODERn] = {RIKI_ENCODER1_TIM, RIKI_ENCODER2_TIM};
const uint16_t ENCODER_A_PIN[ENCODERn] = {RIKI_ENCODER1_A_PIN, RIKI_ENCODER2_A_PIN};
const uint16_t ENCODER_B_PIN[ENCODERn] = {RIKI_ENCODER1_B_PIN, RIKI_ENCODER2_B_PIN};
const uint32_t ENCODER_PORT_CLK[ENCODERn] = {RIKI_ENCODER1_GPIO_CLK, RIKI_ENCODER2_GPIO_CLK};
const uint32_t ENCODER_TIM_CLK[ENCODERn] = {RIKI_ENCODER1_TIM_CLK, RIKI_ENCODER2_TIM_CLK};

Encoder::Encoder(Encoder_TypeDef _encoder, uint32_t _arr, uint32_t _psc)
{
	encoder = _encoder;
	arr = _arr;
	psc = _psc;

	position = 0;
	last_timer = 0;
	last_timer_diff = 0;
}

void Encoder::init()
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	RCC_APB2PeriphClockCmd(ENCODER_PORT_CLK[this->encoder], ENABLE);
	GPIO_InitStructure.GPIO_Pin = ENCODER_A_PIN[this->encoder] | ENCODER_B_PIN[this->encoder];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ENCODER_PORT[this->encoder], &GPIO_InitStructure);

	RCC_APB1PeriphClockCmd(ENCODER_TIM_CLK[this->encoder], ENABLE);
	TIM_DeInit(ENCODER_TIM[this->encoder]);  
	TIM_TimeBaseInit(ENCODER_TIM[this->encoder], &TIM_TimeBaseStructure); 

	TIM_TimeBaseStructure.TIM_Period = this->arr; 
	TIM_TimeBaseStructure.TIM_Prescaler = this->psc;     //设置预分频：  
	TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1;  //设置时钟分频系数：不分频 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式   

	TIM_TimeBaseInit(ENCODER_TIM[this->encoder], &TIM_TimeBaseStructure);

	if(this->encoder == ENCODER1){
		TIM_EncoderInterfaceConfig(ENCODER_TIM[this->encoder], TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Rising);
	} else {
		TIM_EncoderInterfaceConfig(ENCODER_TIM[this->encoder], TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);
	}

	TIM_ICStructInit(&TIM_ICInitStructure); 
	TIM_ICInitStructure.TIM_ICFilter = 6; 
	TIM_ICInit(ENCODER_TIM[this->encoder], &TIM_ICInitStructure);
	
	TIM_ClearFlag(ENCODER_TIM[this->encoder], TIM_FLAG_Update); 
	TIM_ITConfig(ENCODER_TIM[this->encoder], TIM_IT_Update, ENABLE); 

	TIM_SetCounter(ENCODER_TIM[this->encoder], 0);

	TIM_Cmd(ENCODER_TIM[this->encoder], ENABLE); 
}

int32_t Encoder::read()
{
	uint16_t timer_value = TIM_GetCounter(ENCODER_TIM[this->encoder]);
	last_timer_diff = timer_value - last_timer;
	last_timer = timer_value;
	position += (int32_t) last_timer_diff;

	return position;
}

void Encoder::set_pos(int32_t pos)
{
	position = pos;
}
