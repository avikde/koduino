/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "timer.h"
#include "timebase.h"
#include "exti.h"
#include "variant.h"
#include "USARTClass.h"
  
/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  // TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/


// GENERAL-PURPOSE TIMERS 
// Used for analogWrite (PWM), pwmIn
// See timer.c

void TIM2_IRQHandler() {
  timerISR(TIMER2);
}
void TIM5_IRQHandler() {
  timerISR(TIMER5);
}
void TIM3_IRQHandler() {
  timerISR(TIMER3);
}
void TIM4_IRQHandler() {
  timerISR(TIMER4);
}
void TIM1_CC_IRQHandler() {
  timerISR(TIMER1);
}
void TIM8_CC_IRQHandler() {
  timerISR(TIMER8);
}
void TIM1_BRK_TIM9_IRQHandler() {
  timebaseISR(0, TIMER9);
}
void TIM8_BRK_TIM12_IRQHandler() {
  timebaseISR(0, TIMER12);
}

// TIMEBASE TIMERS
// For timekeeping (millis etc.), timer interrupts (attachTimerInterrupt)
// See timebase.c

void TIM1_UP_TIM10_IRQHandler() {
  timebaseISR(0, TIMER10);
}
void TIM1_TRG_COM_TIM11_IRQHandler() {
  timebaseISR(1, TIMER11);
}
void TIM8_UP_TIM13_IRQHandler() {
  timebaseISR(2, TIMER13);
}
void TIM8_TRG_COM_TIM14_IRQHandler() {
  timebaseISR(3, TIMER14);
}
void TIM6_DAC_IRQHandler() {
  timebaseISR(4, TIMER6);
}
void TIM7_IRQHandler() {
  timebaseISR(5, TIMER7);
}

// UNUSED TIMERS
// For future use?

// EXTI
// For attachInterrupt
// See exti.c

void EXTI0_IRQHandler()
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    wirishExternalInterruptHandler(0);
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}
void EXTI1_IRQHandler()
{
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
    wirishExternalInterruptHandler(1);
    EXTI_ClearITPendingBit(EXTI_Line1);
  }
}
void EXTI2_TS_IRQHandler()
{
  if(EXTI_GetITStatus(EXTI_Line2) != RESET)
  {
    wirishExternalInterruptHandler(2);
    EXTI_ClearITPendingBit(EXTI_Line2);
  }
}
void EXTI3_IRQHandler()
{
  if(EXTI_GetITStatus(EXTI_Line3) != RESET)
  {
    wirishExternalInterruptHandler(3);
    EXTI_ClearITPendingBit(EXTI_Line3);
  }
}
void EXTI4_IRQHandler()
{
  if(EXTI_GetITStatus(EXTI_Line4) != RESET)
  {
    wirishExternalInterruptHandler(4);
    EXTI_ClearITPendingBit(EXTI_Line4);
  }
}
void EXTI9_5_IRQHandler()
{
  if(EXTI_GetITStatus(EXTI_Line5) != RESET)
  {
    wirishExternalInterruptHandler(5);
    EXTI_ClearITPendingBit(EXTI_Line5);
  }
  if(EXTI_GetITStatus(EXTI_Line6) != RESET)
  {
    wirishExternalInterruptHandler(6);
    EXTI_ClearITPendingBit(EXTI_Line6);
  }
  if(EXTI_GetITStatus(EXTI_Line7) != RESET)
  {
    wirishExternalInterruptHandler(7);
    EXTI_ClearITPendingBit(EXTI_Line7);
  }
  if(EXTI_GetITStatus(EXTI_Line8) != RESET)
  {
    wirishExternalInterruptHandler(8);
    EXTI_ClearITPendingBit(EXTI_Line8);
  }
  if(EXTI_GetITStatus(EXTI_Line9) != RESET)
  {
    wirishExternalInterruptHandler(9);
    EXTI_ClearITPendingBit(EXTI_Line9);
  }
}
void EXTI15_10_IRQHandler()
{
  if(EXTI_GetITStatus(EXTI_Line10) != RESET)
  {
    wirishExternalInterruptHandler(10);
    EXTI_ClearITPendingBit(EXTI_Line10);
  }
  if(EXTI_GetITStatus(EXTI_Line11) != RESET)
  {
    wirishExternalInterruptHandler(11);
    EXTI_ClearITPendingBit(EXTI_Line11);
  }
  if(EXTI_GetITStatus(EXTI_Line12) != RESET)
  {
    wirishExternalInterruptHandler(12);
    EXTI_ClearITPendingBit(EXTI_Line12);
  }
  if(EXTI_GetITStatus(EXTI_Line13) != RESET)
  {
    wirishExternalInterruptHandler(13);
    EXTI_ClearITPendingBit(EXTI_Line13);
  }
  if(EXTI_GetITStatus(EXTI_Line14) != RESET)
  {
    wirishExternalInterruptHandler(14);
    EXTI_ClearITPendingBit(EXTI_Line14);
  }
  if(EXTI_GetITStatus(EXTI_Line15) != RESET)
  {
    wirishExternalInterruptHandler(15);
    EXTI_ClearITPendingBit(EXTI_Line15);
  }
}

void USART1_IRQHandler() {
  wirishUSARTInterruptHandler(&USART_MAP[0]);
}
void USART2_IRQHandler() {
  wirishUSARTInterruptHandler(&USART_MAP[1]);
}
void USART3_IRQHandler() {
  wirishUSARTInterruptHandler(&USART_MAP[2]);
}
void USART6_IRQHandler() {
  wirishUSARTInterruptHandler(&USART_MAP[3]);
}


/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
