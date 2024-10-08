/*
 * I2C_Program.c
 *
 *  Created on: Dec 10, 2023
 *      Author: Mohanad
 */

#include "stm32f103c8_I2C.h"
#include "stm32f103c8_GPIO.h"

/*
 * =============================================================================
 * 										Generic Variables
 * =============================================================================
 */

I2C_InitTypeDef Global_I2C_Config[2] = {0,0};

/*
 * =============================================================================
 * 										Generic Macros
 * =============================================================================
 */

#define I2C1_INDEX			0
#define I2C2_INDEX			1

/*
 * =============================================================================
 * 										APIs
 * =============================================================================
 */

void MCAL_I2C_Init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct)
{
	uint16_t tmpreg = 0, freqrange = 0;
	uint32_t pclk1 = 8000000;
	uint16_t result = 0 ;

	I2C1->CR1 &= ~I2C_CR1_SWRST;

	//Enable RCC Clock
	if (I2Cx == I2C1)
	{
		Global_I2C_Config[I2C1_INDEX] = *I2C_InitStruct ;
		RCC_I2C1_CLK_EN();
	}else
	{
		Global_I2C_Config[I2C2_INDEX] = *I2C_InitStruct ;
		RCC_I2C2_CLK_EN();
	}
	if (I2C_InitStruct->I2C_Mode == I2C_mode_I2C)
	{
		/*---------------------------- INIT Timing  ------------------------*/
		//---I2C_CR2.FREQ[5:0]: Peripheral clock frequency
		/* Get the I2Cx CR2 value */
		tmpreg = I2Cx->CR2;
		/* Clear frequency FREQ[5:0] bits */
		tmpreg &= ~(I2C_CR2_FREQ_Msk);
		/* Get pclk1 frequency value */
		pclk1 = 8000000;
		/* Set frequency bits depending on pclk1 value */
		freqrange = (uint16_t) (pclk1 / 1000000) ;

		tmpreg |= freqrange ;
		/* Write to I2Cx CR2 */
		I2Cx->CR2 = tmpreg;

		//• Configure the clock control registers (I2C_CCR)
		/* Disable the selected I2C peripheral to configure Time */

		I2Cx->CR1 &= ~(I2C_CR1_PE) ;

		tmpreg = 0;

		/* Configure speed in standard mode */
		if ((I2C_InitStruct->I2C_ClockSpeed == I2C_SCLK_SM_50k)||(I2C_InitStruct->I2C_ClockSpeed == I2C_SCLK_SM_100k) )
		{
			/* Standard mode speed calculate */

			//			Tclk/2 = CRR * Tpclk1
			//			CRR = Tclk /(2 * Tpclk1)
			//			CRR = Fpclk1 / (2 * I2C_ClockFrequency)

			result = (uint16_t) ((pclk1) / ( I2C_InitStruct->I2C_ClockSpeed << 1)   );

			tmpreg |=result ;

			/* Write to I2Cx CCR */
			I2Cx->CCR = tmpreg;

			/*---------------------------- I2C_TRISE Configuration ------------------------*/
			//			For instance: in Sm mode, the maximum allowed SCL rise time is 1000 ns.
			//			If, in the I2C_CR2 register, the value of FREQ[5:0] bits is equal to 0x08 and TPCLk1 = 125 ns
			//			therefore the TRISE[5:0] bits must be programmed with 09h.

			I2Cx->TRISE = freqrange + 1;
		}
		else
		{
			//			Fast Mode Not Supported
		}

		/*---------------------------- I2Cx CR1 Configuration ------------------------*/

		/* Get the I2Cx CR1 value */
		tmpreg = I2Cx->CR1;


		tmpreg |= (uint16_t)(I2C_InitStruct->I2C_ACK_Control | I2C_InitStruct->General_Call_Address_Detection | I2C_InitStruct->I2C_Mode |  I2C_InitStruct->StretchMode );

		/* Write to I2Cx CR1 */
		I2Cx->CR1 = tmpreg;

		/*---------------------------- I2Cx OAR1 & I2Cx OAR2 Configuration -----------------------*/
		tmpreg = 0 ;
		if (I2C_InitStruct->I2C_Slave_Device_Address.Enable_Dual_ADD == 1)
		{
			tmpreg   = I2C_OAR2_ENDUAL ;
			tmpreg  |= I2C_InitStruct->I2C_Slave_Device_Address.Secondary_slave_address << I2C_OAR2_ADD2_Pos ;
			I2Cx->OAR2 = tmpreg;
		}

		tmpreg = 0 ;
		tmpreg  |= I2C_InitStruct->I2C_Slave_Device_Address.Primary_slave_address << 1 ;
		tmpreg  |= I2C_InitStruct->I2C_Slave_Device_Address.I2C_Addressing_Slave_mode ;
		I2Cx->OAR1 = tmpreg;



	}else
	{
		//SMBUS Not Supported
	}

	/* Enable the selected I2C peripheral */
	I2Cx->CR1 |=  I2C_CR1_PE;
}

void MCAL_I2C_RESET()
{
	I2C1->CR1 |= I2C_CR1_SWRST;
}

void MCAL_I2C_DInit(I2C_TypeDef* I2Cx)
{
	if(I2Cx == I2C1)
	{
		NVIC_I2C1_EV_IRQ31_DISABLE;
		NVIC_I2C1_ER_IRQ32_DISABLE;
	}
	else if(I2Cx == I2C2)
	{
		NVIC_I2C2_EV_IRQ33_DISABLE;
		NVIC_I2C2_ER_IRQ34_DISABLE;
	}
}

void MCAL_I2C_GPIO_Set_Pins(I2C_TypeDef* I2Cx)
{
	GPIO_PinConfig_t PinCFG;

	if(I2Cx == I2C1)
	{
		//		PB6 : I2C1_SCL
		//		PB7 : I2C1_SDA

		//Configure SCL of I2C1
		PinCFG.GPIO_PinNumber = GPIO_PIN_6;
		PinCFG.GPIO_Mode = GPIO_MODE_OUTPUT_AF_OD;
		PinCFG.GPIO_Output_Speed = GPIO_SPEED_10M;
		MCAL_GPIO_Init(GPIOB, &PinCFG);

		//Configure SDA of I2C1
		PinCFG.GPIO_PinNumber = GPIO_PIN_7;
		PinCFG.GPIO_Mode = GPIO_MODE_OUTPUT_AF_OD;
		PinCFG.GPIO_Output_Speed = GPIO_SPEED_10M;
		MCAL_GPIO_Init(GPIOB, &PinCFG);
	}
	else if(I2Cx == I2C2)
	{
		//		PB10 : I2C2_SCL
		//		PB11 : I2C2_SDA

		//Configure SCL of I2C2
		PinCFG.GPIO_PinNumber = GPIO_PIN_10;
		PinCFG.GPIO_Mode = GPIO_MODE_OUTPUT_AF_OD;
		PinCFG.GPIO_Output_Speed = GPIO_SPEED_10M;
		MCAL_GPIO_Init(GPIOB, &PinCFG);

		//Configure SDA of I2C2
		PinCFG.GPIO_PinNumber = GPIO_PIN_11;
		PinCFG.GPIO_Mode = GPIO_MODE_OUTPUT_AF_OD;
		PinCFG.GPIO_Output_Speed = GPIO_SPEED_10M;
		MCAL_GPIO_Init(GPIOB, &PinCFG);
	}
}

void MCAL_I2C_MASTER_TX(I2C_TypeDef* I2Cx, uint16_t devAddr, uint8_t *dataOut, uint32_t dataLen, Stop_Condition Stop, Repeated_Start start)
{
	int i = 0;
	//	1. Set the start bit in the I2C_CR1 register to generate a Start condition
	I2C_GenerateSTART(I2Cx, ENABLE, start);

	//  2. wait for EV5
	while(!I2C_GetFlagStatus(I2Cx, EV5));
	//  3. Send address
	I2C_SendAddress(I2Cx, devAddr, I2C_Direction_Transmitter);	// address
	//  4. wait for EV6
	while(!I2C_GetFlagStatus(I2Cx, EV6));
	//  5. wait for EV8_1
	/* TRA, BUSY, MSL, TXE flags */
	while(!I2C_GetFlagStatus(I2Cx, MASTER_BYTE_TRANSMITTING));

	for(i = 0; i < dataLen; i++)
	{
		//  6. wait for EV8
		while(!I2C_GetFlagStatus(I2Cx, EV8));
		/* Write in the DR register the data to be sent */
		if(i >> 1)
		{
			delay_ms(10);
		}
		I2Cx->DR = dataOut[i];
	}

	//  7. wait EV8_2
	//while(!I2C_GetFlagStatus(I2Cx, EV8_2));

	if(Stop == With_Stop)
	{
		//  8. Sent stop condition
		I2C_GenerateSTOP(I2Cx, ENABLE);
	}
}


void MCAL_I2C_MASTER_RX(I2C_TypeDef* I2Cx, uint16_t devAddr, uint8_t *dataOut, uint32_t dataLen, Stop_Condition Stop, Repeated_Start start)
{
	uint8_t index = I2Cx == I2C1? I2C1_INDEX : I2C2_INDEX;
	int i = 0;
	//	1. Set the start bit in the I2C_CR1 register to generate a Start condition
	I2C_GenerateSTART(I2Cx, ENABLE, start);

	//  2. wait for EV5
	while(!I2C_GetFlagStatus(I2Cx, EV5));

	//  3. Send address
	I2C_SendAddress(I2Cx, devAddr, I2C_Direction_Receiver);	// address
	//delay_ms(20);
	//  4. wait for EV6
	while(!I2C_GetFlagStatus(I2Cx, EV6));
	//delay_ms(10);
	I2C_ACKnowledgeConfig(I2Cx,ENABLE);
	//delay_ms(25);
	I2Cx->CR1 &= ~(1 << 11);
	uint8_t sr1 = I2Cx->SR1;
	uint8_t sr2 = I2Cx->SR2;
	if(dataLen)
	{
		// read the data until Len becomes zero
		for(i = dataLen-1; i >= 0; i--)
		{
			//  5. wait for EV7
			while(!I2C_GetFlagStatus(I2Cx, EV7));
			//read the data from data register into buffer
			dataOut[i] = I2Cx->DR;
			//delay_ms(20);
			//increment the buffer address;
			//dataOut++;
		}

		I2C_ACKnowledgeConfig(I2Cx,DISABLE);

	}

	if(Stop == With_Stop)
		//  7. Send Stop condition
		I2C_GenerateSTOP(I2Cx, ENABLE);

	//re-enabling ACKing
	if(Global_I2C_Config[index].I2C_ACK_Control == I2C_Ack_Enable)
	{
		I2C_ACKnowledgeConfig(I2Cx, ENABLE);
	}
}


void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState,Repeated_Start start)
{

	if (start != repeated_start){
		//Check if the BUS is idle
		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
	}
	//	Bit 8 START: Start generation
	//	This bit is set and cleared by software and cleared by hardware when start is sent or PE=0.
	//	In Master Mode:
	//	0: No Start generation
	//	1: Repeated start generation
	//	In Slave mode:
	//	0: No Start generation
	//	1: Start generation when the bus is free

	if (NewState != DISABLE)
	{
		/* Generate a START condition */
		I2Cx->CR1 |= I2C_CR1_START;
	}
	else
	{
		/* Disable the START condition generation */
		I2Cx->CR1 &= ~(I2C_CR1_START) ;
	}

}

FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, Status flag)
{
	//volatile uint32_t dummyRead;
	FlagStatus bitstatus = RESET;
	uint32_t flag1 = 0, flag2 = 0;
	uint32_t lastevent = 0;
	switch(flag)
	{
	case I2C_FLAG_BUSY:
	{
		// Check bit 1 in I2C_SR2
		//0 : Bus is not busy
		//1 : Bus is busy

		if((I2Cx->SR2)&(I2C_SR2_BUSY))
			bitstatus = SET;
		else
			bitstatus = RESET;
		break;
	}
	case EV5:
	{
		// Check bit 0 in I2C_SR1	(start bit)
		//0 : No start condition
		//1 : Start condition generated

		if((I2Cx->SR1)&(I2C_SR1_SB))
			bitstatus = SET;
		else
			bitstatus = RESET;
		break;
	}
	case EV6:
	{
		// chech bit ADDR = 1 (address sent), cleared by reading SR1 followed by reading SR2
		if((I2Cx->SR1)&(I2C_SR1_ADDR))
			bitstatus = SET;
		else
			bitstatus = RESET;

		//to clear ADDR
		//dummyRead = I2Cx -> SR2;
		break;
	}
	case MASTER_BYTE_TRANSMITTING:
	{
		/* Read the I2Cx status register */
		flag1 = I2Cx->SR1;
		flag2 = I2Cx->SR2;
		flag2 = flag2 << 16;
		/* Get the last event value from I2C status register */
		lastevent = (flag1 | flag2) & ((uint32_t)0x00FFFFFF);
		/* Check whether the last event contains the I2C_EVENT */
		if((lastevent & flag) == flag)
		{
			/* SUCCESS: last event is equal to I2C_EVENT */
			bitstatus = SET;
		}
		else
		{
			bitstatus = RESET;
		}
		break;
	}
	case EV8_1:
	case EV8:
	{
		// Bit 7 TxE: Data register empty (transmitters)
		// 0 : Data register not empty
		// 1 : Data register empty
		if(((I2Cx->SR1)&(I2C_SR1_TXE)))
			bitstatus = SET;
		else
			bitstatus = RESET;
		break;
	}
	case EV7:
	{
		// Bit 6 in SR1
		//0 : Data register empty
		//1 : Data register not empty
		if(((I2Cx->SR1)&(I2C_SR1_RXNE)))
			bitstatus = SET;
		else
			bitstatus = RESET;
		break;
	}
	}
	return bitstatus;
}

void I2C_SendAddress(I2C_TypeDef* I2Cx, uint16_t Address, I2C_Direction Direction)
{
	Address = (Address << 1);
	if(Direction != I2C_Direction_Transmitter)
	{
		/* Set the address bit 0 for read */
		Address |= 1<<0;
	}
	else
	{
		/* Set the address bit 0 for write */
		Address &= ~(1<<0);
	}

	/* Send the address */
	I2Cx->DR = Address;
}

void I2C_GenerateSTOP(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
	if(NewState != DISABLE)
	{
		/* Generate a STOP condition */
		I2Cx->CR1 |= I2C_CR1_STOP;
	}
	else
	{
		/* Disable the STOP condition generation */
		I2Cx->CR1 &= ~(I2C_CR1_STOP);
	}
}

void I2C_ACKnowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
	if(NewState != DISABLE)
	{
		/* Enable the ACKnowledgement */
		I2Cx->CR1 |= I2C_CR1_ACK;
	}
	else
	{
		/* Disable the ACKnowledgement */
		I2Cx->CR1 &= ~(I2C_CR1_ACK);
	}

}

// Function to provide a delay in milliseconds
void delay_ms(uint32_t milliseconds) {
	// Assuming a 1 MHz clock frequency for illustration purposes
	// Adjust the loop count based on your actual clock frequency
	// This is a simple and blocking delay function
	// In a real application, you might want to use a timer for more accurate timing

	// Calculate the loop count needed for the specified delay
	uint32_t loop_count = milliseconds * 1000;

	// Perform the delay using a loop
	for (uint32_t i = 0; i < loop_count; i++) {
		// Do nothing
	}
}
