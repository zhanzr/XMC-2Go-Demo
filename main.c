#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <xmc_scu.h>
#include <xmc_rtc.h>
#include <xmc_uart.h>

#include "led.h"
#include "core_portme.h"

#define UART_RX P2_2
#define UART_TX P2_1

XMC_GPIO_CONFIG_t uart_tx;
XMC_GPIO_CONFIG_t uart_rx;

__IO uint32_t g_Ticks;

/* UART configuration */
const XMC_UART_CH_CONFIG_t uart_config = 
{	
  .data_bits = 8U,
  .stop_bits = 1U,
  .baudrate = 921600U
};

XMC_RTC_CONFIG_t rtc_config =
{
  .time.seconds = 5U,
  .prescaler = 0x7fffU
};     

XMC_RTC_TIME_t init_rtc_time = 
{
	.year = 2017,
	.month = XMC_RTC_MONTH_JANUARY,
	.daysofweek = XMC_RTC_WEEKDAY_TUESDAY,
	.days = 17,
	.hours = 5,
	.minutes = 6,
	.seconds = 55	
};

int stdout_putchar (int ch)
{
	XMC_UART_CH_Transmit(XMC_UART0_CH0, (uint8_t)ch);
	return ch;
}

void SysTick_Handler(void) {
  g_Ticks++;
}     

uint32_t HAL_GetTick(void) {
	return g_Ticks;
}

void original_main(void)
{
	__IO uint32_t tmpTick;
	__IO uint32_t deltaTick;
	__IO uint32_t i=0;		
	
//	__IO XMC_RTC_TIME_t now_rtc_time;

  /* System timer configuration */
  SysTick_Config(SystemCoreClock / CLOCKS_PER_SEC);
	
  /*Initialize the UART driver */
	uart_tx.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT6;
	uart_rx.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
 /* Configure UART channel */
  XMC_UART_CH_Init(XMC_UART0_CH0, &uart_config);
  XMC_UART_CH_SetInputSource(XMC_UART0_CH0, XMC_UART_CH_INPUT_RXD1, USIC0_C0_DX3_P2_2);
  
	/* Start UART channel */
  XMC_UART_CH_Start(XMC_UART0_CH0);

  /* Configure pins */
	XMC_GPIO_Init(UART_TX, &uart_tx);
  XMC_GPIO_Init(UART_RX, &uart_rx);
	
	printf("XMC2Go Coremark @ %u Hz %08X %08X %u\n", 
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
	
	LED_Initialize();
	
//	while (1) {
//		/* Convert temperature to Celcius */
//		temp_C = XMC_SCU_CalcTemperature() - 273;

//		g_in_thread_msp = __get_MSP();
//		__NOP();
//		printf("%u, %08X\n", temp_C, g_in_thread_msp);
//		
//		tmpU8 = HAL_GetTick()%0x9;

//		lockTick = HAL_GetTick();
//		while((lockTick+4000) > HAL_GetTick())
//		{
//			__NOP();
//		}	
//  }
}
