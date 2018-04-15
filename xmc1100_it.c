#include <XMC1100.h>
#include <stdio.h>

void HardFault_Handler(void)
{
	printf(__func__);
	while(1)
	{;}
}

void SVC_Handler(uint32_t dummy, uint32_t sn)
{
	printf("%s>%u\n", __func__, sn);
	
	//This line will induce a hardfault		
	__ASM("SVC #0");
}

void PendSV_Handler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}

void SCU_0_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}

void SCU_1_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}
                      
void SCU_2_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}
void ERU0_0_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}

void ERU0_1_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}
                   
void ERU0_2_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}

void ERU0_3_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}

__weak void USIC0_0_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}        
       
void USIC0_1_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}

void USIC0_2_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}

void USIC0_3_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}        
       
void USIC0_4_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}

void USIC0_5_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}

void VADC0_C0_0_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}      
      
void VADC0_C0_1_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}

void CCU40_0_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}

void CCU40_1_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}   
      
void CCU40_2_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
}

void CCU40_3_IRQHandler(void)
{
	printf(__FUNCTION__);
	while(1)
	{;}
} 
