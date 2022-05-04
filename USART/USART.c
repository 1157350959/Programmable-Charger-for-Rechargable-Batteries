# include "./USART/USART.h"

void USART_Config(void){
	
	/* define basic structures */
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* enable GPIOA clock since PA2 will be UART2 TX, and PA3 will be UART2 RX */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	/* enable USART2 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	/* Alternate Function mapping: GPIO2<->USART2_TX, GPIO3<->USART3_RX */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	/* configure PA2 and PA3 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* configure USART2 */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode= USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity= USART_Parity_No;
	USART_InitStructure.USART_StopBits= USART_StopBits_1;
	USART_InitStructure.USART_WordLength= USART_WordLength_8b;
	
	USART_Init(USART2, &USART_InitStructure);
	
	/* enable USART2 */
	USART_Cmd(USART2, ENABLE);
}

void USART_SendByte(USART_TypeDef *USARTx, uint16_t ch){
	/* re-encapsulate the USART_SendData together with flag checking to avoid overwriting in quick transmissions */
	USART_SendData(USARTx, ch);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}
	
void USART_SendString(USART_TypeDef *USARTx, char *str){
	unsigned int cnt = 0;
	while (*(str + cnt) != '\0'){
		USART_SendByte(USARTx, *(str + cnt));
		cnt ++;
	}
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

int fputc(int ch, FILE *f){
	USART_SendData(USART2, (uint8_t) ch);
	
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	
	return (ch);
}

int fgetc(FILE *f){
	while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
	
	return (int)USART_ReceiveData(USART2);
}
