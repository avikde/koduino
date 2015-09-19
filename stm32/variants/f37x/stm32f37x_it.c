

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
