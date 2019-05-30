#ifdef __cplusplus
extern "C" {
#endif

#include "sonar.h"

uint32_t cnt;

void sonar_init(uint32_t _arr, uint32_t _psc)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB2PeriphClockCmd(RIKI_SONAR_GPIO_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = RIKI_TRIG_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RIKI_SONAR_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = RIKI_ECHO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(RIKI_SONAR_GPIO_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(RIKI_SONAR_GPIO_PORT, RIKI_ECHO_PIN);

	RCC_APB1PeriphClockCmd(RIKI_SONAR_TIM_CLK, ENABLE);

	TIM_DeInit(RIKI_SONAR_TIM);  
	TIM_TimeBaseStructure.TIM_Period = _arr; 
	TIM_TimeBaseStructure.TIM_Prescaler = _psc;     //设置预分频：  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //设置时钟分频系数 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式   
	TIM_TimeBaseInit(RIKI_SONAR_TIM, &TIM_TimeBaseStructure);
	TIM_ITConfig(RIKI_SONAR_TIM, TIM_IT_Update, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = RIKI_SONAR_TIM_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(RIKI_SONAR_TIM, DISABLE);
}

float get_distance(void)
{
	int i;
	float distance;
	uint32_t measure;

	GPIO_SetBits(RIKI_SONAR_GPIO_PORT, RIKI_TRIG_PIN);
	for(i = 0; i < 2000; i++);
	GPIO_ResetBits(RIKI_SONAR_GPIO_PORT, RIKI_TRIG_PIN);
	while(GPIO_ReadInputDataBit(RIKI_SONAR_GPIO_PORT, RIKI_ECHO_PIN) == RESET);  // 等待高电平回波
	TIM_Cmd(RIKI_SONAR_TIM, ENABLE);  
	while(GPIO_ReadInputDataBit(RIKI_SONAR_GPIO_PORT, RIKI_ECHO_PIN) == SET); 
	TIM_Cmd(RIKI_SONAR_TIM, DISABLE);  
	measure = TIM_GetCounter(RIKI_SONAR_TIM);
	TIM_SetCounter(RIKI_SONAR_TIM, 0); 
	distance = (measure + cnt*1000) / 58.8;
	cnt = 0;

	return distance;
	
}

void TIM6_IRQHandler(void)
{
	if(TIM_GetITStatus(RIKI_SONAR_TIM, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(RIKI_SONAR_TIM, TIM_IT_Update);
		cnt++;	
	}
}

#ifdef __cplusplus
}
#endif
