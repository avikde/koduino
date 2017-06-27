/**
 * @authors Avik De <avikde@gmail.com>

  This file is part of koduino <https://github.com/avikde/koduino>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#include "timer.h"
#include "timebase.h"
#include "exti.h"
#include "variant.h"
#include "USARTClass.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/******************************************************************************/
/*            Cortex-M Processor Exceptions Handlers                         */
/******************************************************************************/

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : MemManage_Handler
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : BusFault_Handler
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : UsageFault_Handler
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : SVC_Handler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVC_Handler(void)
{
}

/*******************************************************************************
* Function Name  : DebugMon_Handler
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMon_Handler(void)
{
}

/*******************************************************************************
* Function Name  : PendSV_Handler
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSV_Handler(void)
{
}


/******************************************************************************/
/*                 STM32 Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32xxx.s).                                            */
/******************************************************************************/

/*******************************************************************************
* Function Name  : PPP_IRQHandler
* Description    : This function handles PPP interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*void PPP_IRQHandler(void)
{
}*/

// General purpose timers
void TIM1_CC_IRQHandler() {
  timerISR(TIMER1, 0xff);
}
void TIM2_IRQHandler() {
  timerISR(TIMER2, TIMER2);
}
void TIM3_IRQHandler() {
  timerISR(TIMER3, TIMER3);
}
void TIM4_IRQHandler() {
  timerISR(TIMER4, TIMER4);
}
void TIM8_CC_IRQHandler() {
  timerISR(TIMER8, 0xff);
}
void TIM8_UP_IRQHandler() {
  timerISR(0xff, TIMER8);
}
void TIM1_BRK_TIM15_IRQHandler() {
  timerISR(TIMER15, TIMER15);
}
void TIM1_UP_TIM16_IRQHandler() {
  timerISR(TIMER16, TIMER1);
}
void TIM1_TRG_COM_TIM17_IRQHandler() {
  timerISR(TIMER17, TIMER17);
}

// Timebase
void TIM6_DAC_IRQHandler() {
  timebaseISR(0, TIMER6);
}
void TIM7_IRQHandler() {
  timebaseISR(1, TIMER7);
}

// EXTI

// USART
void USART1_IRQHandler() {
  wirishUSARTInterruptHandler(&USART_MAP[0]);
}
void USART2_IRQHandler() {
  wirishUSARTInterruptHandler(&USART_MAP[1]);
}
void USART3_IRQHandler() {
  wirishUSARTInterruptHandler(&USART_MAP[2]);
}
// UART
void UART4_IRQHandler() {
  wirishUSARTInterruptHandler(&USART_MAP[3]);
}
void UART5_IRQHandler() {
  wirishUSARTInterruptHandler(&USART_MAP[4]);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

