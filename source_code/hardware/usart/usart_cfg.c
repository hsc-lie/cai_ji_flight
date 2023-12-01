#include "usart_cfg.h"

#include "at32f4xx_usart.h"

            
uint8_t USART2_Buffer[USART2_BUFFER_SIZE];
CircularQueue_t USART2_Queue = 
{
	.Buffer = USART2_Buffer,
	.BufferSize = USART2_BUFFER_SIZE,

	.WriteIndex = 0,
	.ReadIndex = 0,
};





void USART_ConfigInitAll()
{
	USART_InitType USART_InitStructure;
	NVIC_InitType NVIC_InitStructure;

	/*************************USART1********************************/
#if TRUE
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);

	//USART_INTConfig(USART1, USART_INT_RDNE, ENABLE);
	//USART_INTConfig(USART1, USART_INT_TDE, ENABLE);

	USART_Cmd(USART1, ENABLE);
	/*
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);*/

#endif

	/*************************USART2********************************/
#if TRUE	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;     //USART_Parity_No  USART_Parity_Even
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx;

  	USART_Init(USART2, &USART_InitStructure);
  
  	//USART_INTConfig(USART2, USART_INT_RDNE, ENABLE);
	//USART_INTConfig(USART2, USART_INT_IDLEF, ENABLE);
  	//USART_INTConfig(UARTx, USART_INT_TDE, ENABLE);

  	USART_Cmd(USART2, ENABLE);
	
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
  
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

#endif

	
}




void USART_TransmissionData(USART_Type * USARTx,  uint8_t * data, uint32_t len)
{
	uint32_t i;
	for(i = 0;i < len;i++)
	{
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TDE) == RESET);
    	USART_SendData(USARTx, *data);
		++data;
	}
}



