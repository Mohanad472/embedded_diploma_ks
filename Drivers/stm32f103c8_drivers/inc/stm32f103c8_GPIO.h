/*
 * stm32f103c8_GPIO.h
 *
 *  Created on: Feb 29, 2024
 *      Author: Mohanad
 */

#ifndef INC_STM32F103C8_GPIO_H_
#define INC_STM32F103C8_GPIO_H_

#include "stm32f103x8.h"

//Config Structure
typedef struct
{
	uint16_t GPIO_PinNumber;		//Specifies the GPIO pins to be configured.
									//This parameter must be set based on @ref GPIO_PINS_define

	uint8_t	GPIO_Mode;				//Specifies the operation mode of the selected pin.
									//This parameter must be set based on @ref GPIO_MODE_define

	uint8_t	GPIO_Output_Speed;		//Specifies the speed for the selected pin.
									//This parameter must be set based on @ref GPIO_SPEED_define
}GPIO_PinConfig_t;


//@ref GPIO_PIN_state
#define GPIO_PIN_SET		1
#define GPIO_PIN_RESET		0

//@ref GPIO_RETURN_LOCK
#define GPIO_RETRN_LOCK_Enabled			0
#define GPIO_RETURN_LOCK_ERROR			1


//================================
// Macros Configuration References
//================================

//@ref GPIO_PINS_define
#define GPIO_PIN_0				((uint16_t)0x0001)	// Pin 0 selected
#define GPIO_PIN_1				((uint16_t)0x0002)	// Pin 1 selected
#define GPIO_PIN_2				((uint16_t)0x0004)	// Pin 2 selected
#define GPIO_PIN_3				((uint16_t)0x0008)	// Pin 3 selected
#define GPIO_PIN_4				((uint16_t)0x0010)	// Pin 4 selected
#define GPIO_PIN_5				((uint16_t)0x0020)	// Pin 5 selected
#define GPIO_PIN_6				((uint16_t)0x0040)	// Pin 6 selected
#define GPIO_PIN_7				((uint16_t)0x0080)	// Pin 7 selected
#define GPIO_PIN_8				((uint16_t)0x0100)	// Pin 8 selected
#define GPIO_PIN_9				((uint16_t)0x0200)	// Pin 9 selected
#define GPIO_PIN_10				((uint16_t)0x0400)	// Pin 10 selected
#define GPIO_PIN_11				((uint16_t)0x0800)	// Pin 11 selected
#define GPIO_PIN_12				((uint16_t)0x1000)	// Pin 12 selected
#define GPIO_PIN_13				((uint16_t)0x2000)	// Pin 13 selected
#define GPIO_PIN_14				((uint16_t)0x4000)	// Pin 14 selected
#define GPIO_PIN_15				((uint16_t)0x8000)	// Pin 15 selected
#define GPIO_PIN_ALL			((uint16_t)0xFFFF)	// All Pins selected


//@ref GPIO_MODE_define
//0: Analog mode
//1: Floating input
//2: Input with pull-up
//3: Input with pull-down
//4: General purpose output push-pull
//5: General purpose output open-drain
//6: Alternate function output push-pull
//7: Alternate function output open-drain
//8: Alternate function input
#define GPIO_MODE_Analog				0x00000000U		// Analog mode
#define GPIO_MODE_INPUT_FLO				0x00000001U		// Floating input
#define GPIO_MODE_INPUT_PU				0x00000002U		// Input with pull-up
#define GPIO_MODE_INPUT_PD				0x00000003U		// Input with pull-down
#define GPIO_MODE_OUTPUT_PP				0x00000004U		// General purpose output push-pull
#define GPIO_MODE_OUTPUT_OD				0x00000005U		// General purpose output open-drain
#define GPIO_MODE_OUTPUT_AF_PP			0x00000006U		// Alternate function output push-pull
#define GPIO_MODE_OUTPUT_AF_OD			0x00000007U		// Alternate function output open-drain
#define GPIO_MODE_AF_INPUT				0x00000008U		// Alternate function input


//@ref GPIO_SPEED_define
//1: Output mode, max speed = 10 MHz.
//2: Output mode, max speed = 2 MHz.
//3: Output mode, max speed = 50 MHz.
#define GPIO_SPEED_10M				    0x00000001U		// Output mode, max speed = 10 MHz.
#define GPIO_SPEED_2M				    0x00000002U		// Output mode, max speed = 2 MHz.
#define GPIO_SPEED_50M				    0x00000003U		// Output mode, max speed = 50 MHz.


/*
 *  ==============================================================
 *  		  APIs Supported by "MCAL GPIO DRIVER"
 *  ==============================================================
 */

void MCAL_GPIO_Init(GPIO_Typedef *GPIOx, GPIO_PinConfig_t* GPIO_PinConfig);
void MCAL_GPIO_DeInit(GPIO_Typedef *GPIOx);
uint8_t MCAL_GPIO_ReadPin(GPIO_Typedef *GPIOx, uint16_t PinNumber);
uint16_t MCAL_GPIO_ReadPort(GPIO_Typedef *GPIOx);
void MCAL_GPIO_WritePin(GPIO_Typedef *GPIOx, uint16_t PinNumber, uint8_t Value);
void MCAL_GPIO_WritePort(GPIO_Typedef *GPIOx, uint16_t Value);
void MCAL_GPIO_TogglePin(GPIO_Typedef *GPIOx, uint16_t PinNumber);
uint8_t MCAL_GPIO_LockPin(GPIO_Typedef *GPIOx, uint16_t PinNumber);

#endif /* INC_STM32F103C8_GPIO_H_ */
