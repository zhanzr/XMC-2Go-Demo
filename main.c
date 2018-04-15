#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <xmc_scu.h>

#include "Driver_USART.h"
#include "xmc_uart.h"
#include "Board_LED.h"

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

uint32_t g_in_handler_msp;
void SysTick_Handler(void)
{
//  g_Ticks++;
	
	g_in_handler_msp = __get_MSP();
	__NOP();
//	printf("gMSP:%08X\n", g_in_handler_msp);
}     

uint32_t HAL_GetTick(void)
{
	return g_Ticks;
}

void testFunc(void)
{
	printf("MSP:%08X\n", __get_MSP());
}

uint32_t g_in_thread_msp;
uint32_t lockTick;
uint32_t temp_C;
int main(void)
{
	/* Enable DTS */
	XMC_SCU_StartTempMeasurement();
	
//  SysTick_Config(SystemCoreClock / 1000);
  SysTick_Config(SystemCoreClock / 2);
	
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
	
	printf("XMC2Go Fault Test demo @ %u Hz %08X %08X %u\n", 
	SystemCoreClock, 
	SCB->CPUID, 
	SCB->CCR,
	__ARMCC_VERSION);
	
	#ifdef __MICROLIB
	printf("With Microlib\n");
	#else
	printf("With StandardLib\n");
	#endif
	
//	printf("Unalignment Access\n");
//	uint8_t testU8_Arr[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
	//These accesses will generate hardfault
//	printf("%04X\n", *(uint16_t*)(testU8_Arr + 1));
//	printf("%08X\n", *(uint32_t*)(testU8_Arr + 1));
	
//	printf("Stack Alignment\n");
//	g_in_thread_msp = __get_MSP();
//	printf("MSP:%08X\n", g_in_thread_msp);
//	__set_MSP(g_in_thread_msp-1);
//	g_in_thread_msp = __get_MSP();
//	printf("MSP:%08X\n", g_in_thread_msp);
//	__set_MSP(g_in_thread_msp-2);
//	g_in_thread_msp = __get_MSP();
//	printf("MSP:%08X\n", g_in_thread_msp);
//	__set_MSP(g_in_thread_msp-3);
//	g_in_thread_msp = __get_MSP();
//	printf("MSP:%08X\n", g_in_thread_msp);
//		__set_MSP(g_in_thread_msp-4);
//	g_in_thread_msp = __get_MSP();
//	printf("MSP:%08X\n", g_in_thread_msp);
	
//	testFunc();
	
	while (1)
  {
		/* Convert temperature to Celcius */
//		temp_C = XMC_SCU_CalcTemperature() - 273;

		g_in_thread_msp = __get_MSP();
		__NOP();
//		printf("%08X\n",g_in_thread_msp);
//				
//		lockTick = HAL_GetTick();
//		while((lockTick+4) > HAL_GetTick())
//		{
//			__NOP();
//		}	
  }
}
