

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
}

/******************************************************************************/
/*                 STM32F37x Peripherals Interrupt Handlers                   */
/******************************************************************************/

/******************************************************************************/
/*                 STM32F37x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f37x.s).                                               */
/******************************************************************************/

// ST interrupts =========================================================================

// GENERAL-PURPOSE TIMERS 
// Used for analogWrite (PWM), pwmIn
// See timer.c

void TIM2_IRQHandler() {
  timerISR(TIMER2);
}

void TIM3_IRQHandler() {
  timerISR(TIMER3);
}

void TIM4_IRQHandler() {
  timerISR(TIMER4);
}

void TIM5_IRQHandler() {
  timerISR(TIMER5);
}

void TIM12_IRQHandler() {
  timerISR(TIMER12);
}

void TIM19_IRQHandler() {
  timerISR(TIMER19);
}

// TIMEBASE TIMERS
// For timekeeping (millis etc.), timer interrupts (attachTimerInterrupt)
// See timebase.c

void TIM6_DAC1_IRQHandler() {
  timebaseISR(0, TIMER6);
}
void TIM7_IRQHandler() {
  timebaseISR(1, TIMER7);
}
void TIM16_IRQHandler() {
  timebaseISR(2, TIMER16);
}
void TIM17_IRQHandler() {
  timebaseISR(3, TIMER17);
}
void TIM18_DAC2_IRQHandler() {
  timebaseISR(4, TIMER18);
}

// UNUSED TIMERS
// For future use?

void TIM13_IRQHandler() {
}

void TIM14_IRQHandler() {
}

void TIM15_IRQHandler() {
}

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


/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
