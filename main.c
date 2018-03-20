#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <xmc_scu.h>

#include "Driver_USART.h"
#include "xmc_uart.h"
#include "Board_LED.h"
#include "XMC1000_TSE.h"

extern ARM_DRIVER_USART Driver_USART0;
static ARM_DRIVER_USART *UARTdrv = &Driver_USART0; 
__IO uint32_t g_Ticks;

void UART_cb(uint32_t event)
{
    switch (event)
    {
    case ARM_USART_EVENT_RECEIVE_COMPLETE:  
     break;
     
    case ARM_USART_EVENT_TRANSFER_COMPLETE:
    case ARM_USART_EVENT_SEND_COMPLETE:
    case ARM_USART_EVENT_TX_COMPLETE:
        break;
 
    case ARM_USART_EVENT_RX_TIMEOUT:
		/* Error: Call debugger or replace with custom error handling */
        break;
 
    case ARM_USART_EVENT_RX_OVERFLOW:
    case ARM_USART_EVENT_TX_UNDERFLOW:
		default:
		/* Error: Call debugger or replace with custom error handling */
        break;
    }
}
  
int stdout_putchar(int ch)
{
	XMC_UART_CH_Transmit(XMC_UART0_CH0, ch);
	return ch;
}

void SysTick_Handler(void)
{
  g_Ticks++;
}     

uint32_t HAL_GetTick(void)
{
	return g_Ticks;
}

void TestFunct1(uint32_t* pA, uint32_t* pB, uint32_t* pC)
{
	*pA += *pC;
	*pB += *pC;
}

void TestFunct2(uint32_t* restrict pA, uint32_t* restrict pB, uint32_t* restrict pC)
{
	*pA += *pC;
	*pB += *pC;
}

int main(void)
{
	uint32_t lockTick;
	uint32_t temp_k;
	uint32_t temp_C;

	/* Enable DTS */
	XMC_SCU_StartTempMeasurement();
	
  SysTick_Config(SystemCoreClock / 1000);
	
  /*Initialize the UART driver */
  UARTdrv->Initialize(UART_cb);
  UARTdrv->PowerControl(ARM_POWER_FULL);
  UARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                   ARM_USART_DATA_BITS_8 |
                   ARM_USART_PARITY_NONE |
                   ARM_USART_STOP_BITS_1 , 256000);
   
  /* Enable the Transmitter/Receiver line */
  UARTdrv->Control (ARM_USART_CONTROL_TX, 1);
  UARTdrv->Control (ARM_USART_CONTROL_RX, 1);
	  
	LED_Initialize();
	
	printf("XMC2Go restrict demo @ %u Hz %08X %u\n", 
	SystemCoreClock, 
	SCB->CPUID, 
	__ARMCC_VERSION);
	
	#ifdef __MICROLIB
	printf("With Microlib\n");
	#else
	printf("With StandardLib\n");
	#endif
	
	/* Calculate temperature of the chip in Kelvin */
	temp_k = XMC1000_CalcTemperature();
	/* Convert temperature to Celcius */
	temp_C = temp_k - 273;	
	printf("TSE_I=%u 'C\n",temp_C);
	
	{
		uint32_t tA = 10;
		uint32_t tB = 20;
		uint32_t tC = 30;
		
		TestFunct1(&tA, &tB, &tC);
		printf("%u %u %u\n",
		tA, tB, tC);
	}
	
	{
		uint32_t tA = 10;
		uint32_t tB = 20;
		uint32_t tC = 30;
		
		TestFunct2(&tA, &tB, &tC);
		printf("%u %u %u\n",
		tA, tB, tC);
	}
	
	while (1)
  {
		/* Calculate temperature of the chip in Kelvin */
		temp_k = XMC1000_CalcTemperature();

		/* Convert temperature to Celcius */
		temp_C = temp_k - 273;

		printf("TSE_I=%u 'C\n",temp_C);
				
    LED_On(0);
    LED_On(1);
		
		lockTick = HAL_GetTick();
		while((lockTick+40000) > HAL_GetTick())
		{
			__NOP();
			__WFI();
		}
		
    LED_Off(0);
    LED_Off(1);
		
		lockTick = HAL_GetTick();
		while((lockTick+40000) > HAL_GetTick())
		{
			__NOP();
			__WFI();
		}		
  }
}
