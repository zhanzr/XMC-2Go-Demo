#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <xmc_scu.h>
#include <xmc_rtc.h>
#include <xmc_uart.h>

#include "led.h"
#include "XMC1000_TSE.h"


#include "arm_math.h"
#include "math_helper.h"

#define SNR_THRESHOLD   90

#define	MAT_R_A	4
#define	MAT_C_A	4
#define	MAT_R_B	4
#define	MAT_C_B	1

#ifndef HZ
#define HZ	1000
#endif

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
  .baudrate = 921600
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


/* --------------------------------------------------------------------------------
* Test input data(Cycles) taken from FIR Q15 module for differant cases of blockSize
* and tapSize
* --------------------------------------------------------------------------------- */

const float32_t B_f32[4] =
{
  782.0, 7577.0, 470.0, 4505.0
};

/* --------------------------------------------------------------------------------
* Formula to fit is  C1 + C2 * numTaps + C3 * blockSize + C4 * numTaps * blockSize
* -------------------------------------------------------------------------------- */

const float32_t A_f32[16] =
{
  /* Const,   numTaps,   blockSize,   numTaps*blockSize */
  1.0,     32.0,      4.0,     128.0,
  1.0,     32.0,     64.0,    2048.0,
  1.0,     16.0,      4.0,      64.0,
  1.0,     16.0,     64.0,    1024.0,
};


/* ----------------------------------------------------------------------
* Temporary buffers  for storing intermediate values
* ------------------------------------------------------------------- */
/* Transpose of A Buffer */
float32_t AT_f32[16];
/* (Transpose of A * A) Buffer */
float32_t ATMA_f32[16];
/* Inverse(Transpose of A * A)  Buffer */
float32_t ATMAI_f32[16];
/* Test Output Buffer */
float32_t X_f32[4];

/* ----------------------------------------------------------------------
* Reference ouput buffer C1, C2, C3 and C4 taken from MATLAB
* ------------------------------------------------------------------- */
const float32_t xRef_f32[4] = {73.0, 8.0, 21.25, 2.875};

float32_t snr;


int stdout_putchar (int ch) {
	XMC_UART_CH_Transmit(XMC_UART0_CH0, (uint8_t)ch);
	return ch;
}

void ttywrch (int ch)
{
	XMC_UART_CH_Transmit(XMC_UART0_CH0, (uint8_t)ch);
}

void SysTick_Handler(void) {
  g_Ticks++;
}     

uint32_t HAL_GetTick(void) {
	return g_Ticks;
}

int main(void)
{
	__IO uint32_t tmpTick;
	__IO uint32_t deltaTick;
	__IO uint32_t lockTick;
	bool testRes;

		/* Enable DTS */
	XMC_SCU_StartTempMeasurement();
	
  /* System timer configuration */
  SysTick_Config(SystemCoreClock / HZ);
	
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
	
	printf("XMC2Go @ %u Hz %08X %08X %u\n", 
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
	


  arm_matrix_instance_f32 A;      /* Matrix A Instance */
  arm_matrix_instance_f32 AT;     /* Matrix AT(A transpose) instance */
  arm_matrix_instance_f32 ATMA;   /* Matrix ATMA( AT multiply with A) instance */
  arm_matrix_instance_f32 ATMAI;  /* Matrix ATMAI(Inverse of ATMA) instance */
  arm_matrix_instance_f32 B;      /* Matrix B instance */
  arm_matrix_instance_f32 X;      /* Matrix X(Unknown Matrix) instance */

  uint32_t srcRows, srcColumns;  /* Temporary variables */
  arm_status status;

  /* Initialise A Matrix Instance with numRows, numCols and data array(A_f32) */
  srcRows = 4;
  srcColumns = 4;
  arm_mat_init_f32(&A, srcRows, srcColumns, (float32_t *)A_f32);

	printf("Initialise A Matrix Instance\n");
	printf("Matrix A:\n");
	for(uint8_t i=0; i<srcRows; ++i)
	{
		for(uint8_t j=0; j<srcColumns; ++j)
		{
			printf("%.1f,\t", A.pData[i*srcColumns + j]);
		}
		printf("\n");
	}	
	printf("\n");
  
	/* Initialise Matrix Instance AT with numRows, numCols and data array(AT_f32) */
  srcRows = 4;
  srcColumns = 4;
  arm_mat_init_f32(&AT, srcRows, srcColumns, AT_f32);

  /* calculation of A transpose */
  status = arm_mat_trans_f32(&A, &AT);

	printf("calculation of A transpose\n");
	printf("Matrix AT:\n");
	for(uint8_t i=0; i<srcRows; ++i)
	{
		for(uint8_t j=0; j<srcColumns; ++j)
		{
			printf("%.1f,\t", AT.pData[i*srcColumns + j]);
		}
		printf("\n");
	}	
	printf("\n");
	
  /* Initialise ATMA Matrix Instance with numRows, numCols and data array(ATMA_f32) */
  srcRows = 4;
  srcColumns = 4;
  arm_mat_init_f32(&ATMA, srcRows, srcColumns, ATMA_f32);

  /* calculation of AT Multiply with A */
  status = arm_mat_mult_f32(&AT, &A, &ATMA);
	
	printf("calculation of AT Multiply with A\n");
	printf("Matrix ATMA:\n");
	for(uint8_t i=0; i<srcRows; ++i)
	{
		for(uint8_t j=0; j<srcColumns; ++j)
		{
			printf("%.1f,\t", ATMA.pData[i*srcColumns + j]);
		}
		printf("\n");
	}	
	printf("\n");
	
  /* Initialise ATMAI Matrix Instance with numRows, numCols and data array(ATMAI_f32) */
  srcRows = 4;
  srcColumns = 4;
  arm_mat_init_f32(&ATMAI, srcRows, srcColumns, ATMAI_f32);

  /* calculation of Inverse((Transpose(A) * A) */
  status = arm_mat_inverse_f32(&ATMA, &ATMAI);
	
	printf("calculation of Inverse((Transpose(A) * A)\n");
	printf("Matrix ATMAI:\n");
	for(uint8_t i=0; i<srcRows; ++i)
	{
		for(uint8_t j=0; j<srcColumns; ++j)
		{
			printf("%.1f,\t", ATMAI.pData[i*srcColumns + j]);
		}
		printf("\n");
	}	
	printf("\n");
	
  /* calculation of (Inverse((Transpose(A) * A)) *  Transpose(A)) */
  status = arm_mat_mult_f32(&ATMAI, &AT, &ATMA);
	printf("calculation of (Inverse((Transpose(A) * A)) *  Transpose(A))\n");
	printf("Matrix ATMAI:\n");
	for(uint8_t i=0; i<srcRows; ++i)
	{
		for(uint8_t j=0; j<srcColumns; ++j)
		{
			printf("%.1f,\t", ATMA.pData[i*srcColumns + j]);
		}
		printf("\n");
	}	
	printf("\n");
	
  /* Initialise B Matrix Instance with numRows, numCols and data array(B_f32) */
  srcRows = 4;
  srcColumns = 1;
  arm_mat_init_f32(&B, srcRows, srcColumns, (float32_t *)B_f32);
	printf("Initialise B Matrix\n");
	printf("Matrix B:\n");
	for(uint8_t i=0; i<srcRows; ++i)
	{
		for(uint8_t j=0; j<srcColumns; ++j)
		{
			printf("%.1f,\t", B.pData[i*srcColumns + j]);
		}
		printf("\n");
	}	
	printf("\n");
  /* Initialise X Matrix Instance with numRows, numCols and data array(X_f32) */
  srcRows = 4;
  srcColumns = 1;
  arm_mat_init_f32(&X, srcRows, srcColumns, X_f32);

  /* calculation ((Inverse((Transpose(A) * A)) *  Transpose(A)) * B) */
  status = arm_mat_mult_f32(&ATMA, &B, &X);
	printf("calculation ((Inverse((Transpose(A) * A)) *  Transpose(A)) * B)\n");
	printf("Matrix X:\n");
	for(uint8_t i=0; i<srcRows; ++i)
	{
		for(uint8_t j=0; j<srcColumns; ++j)
		{
			printf("%f, ", X.pData[i*srcColumns + j]);
		}
		printf("\n");
	}	
	printf("\n");
	
	printf("Ref of Matlab:\n");
	for(uint8_t i=0; i<srcRows; ++i)
	{
		for(uint8_t j=0; j<srcColumns; ++j)
		{
			printf("%f, ", xRef_f32[i*srcColumns + j]);
		}
		printf("\n");
	}	
	printf("\n");	
  /* Comparison of reference with test output */
  snr = arm_snr_f32((float32_t *)xRef_f32, X_f32, 4);

  /*------------------------------------------------------------------------------
  *            Initialise status depending on SNR calculations
  *------------------------------------------------------------------------------*/
  if( snr > SNR_THRESHOLD)
  {
    status = ARM_MATH_SUCCESS;
  }
  else
  {
    status = ARM_MATH_TEST_FAILURE;
  }

	printf("Result snr:%.4f\n", snr);

  /* ----------------------------------------------------------------------
  ** Loop here if the signals fail the PASS check.
  ** This denotes a test failure
  ** ------------------------------------------------------------------- */
  if( status != ARM_MATH_SUCCESS)
  {
    while(1);
  }
	
	while (1) {
		/* Convert temperature to Celcius */
		int32_t temp_C = (int32_t)XMC_SCU_CalcTemperature() - 273;
		printf("temperature: %i\n", temp_C);
		
		lockTick = HAL_GetTick();
		while((lockTick + 2*HZ) > HAL_GetTick())
		{
			__NOP();
		}	
  }
}
