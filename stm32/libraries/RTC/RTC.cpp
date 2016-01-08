#include "RTC.h"

RTCClass::RTCClass(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIOs clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  /* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure MCO (PA8) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Output LSE clock (32.768KHz) on MCO pin (PA8) */
	RCC_MCO1Config(RCC_MCO1Source_LSE, RCC_MCO1Div_1);		
}

void RTCClass::setTime(time_t localTime) {

	RTC_InitTypeDef RTC_InitStructure;
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;

	struct tm * timeinfo;
	timeinfo = gmtime (&localTime);


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	PWR_BackupAccessCmd(ENABLE);
	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);

    //   /* Enable the LSE OSC */
	RCC_LSICmd(DISABLE);
	RCC_LSEConfig(RCC_LSE_ON);

      /* Wait till LSE is ready */ 
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}

      /* Select the LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
     /* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/

    /* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();

    /* Write to the first RTC Backup Data Register */
	RTC_WriteBackupRegister(RTC_BKP_DR0, 0xA5A5);

	/* Set the Time */
	RTC_TimeStructure.RTC_Hours   = timeinfo->tm_hour;
	RTC_TimeStructure.RTC_Minutes = timeinfo->tm_min;
	RTC_TimeStructure.RTC_Seconds = timeinfo->tm_sec;

  	/* Set the Date */
  	switch (timeinfo->tm_mon) {
  		case 0:
  			RTC_DateStructure.RTC_Month = RTC_Month_January;
  			break;
  		case 1:
  			RTC_DateStructure.RTC_Month = RTC_Month_February;
  			break;
  		case 2:
  			RTC_DateStructure.RTC_Month = RTC_Month_March;
  			break;
  		case 3:
  			RTC_DateStructure.RTC_Month = RTC_Month_April;
  			break;
  		case 4:
  			RTC_DateStructure.RTC_Month = RTC_Month_May;
  			break;
  		case 5:
  			RTC_DateStructure.RTC_Month = RTC_Month_June;
  			break;
  		case 6:
  			RTC_DateStructure.RTC_Month = RTC_Month_July;
  			break;
  		case 7:
  			RTC_DateStructure.RTC_Month = RTC_Month_August;
  			break;
  		case 8:
  			RTC_DateStructure.RTC_Month = RTC_Month_September;
  			break;
  		case 9:
  			RTC_DateStructure.RTC_Month = RTC_Month_October;
  			break;
  		case 10:
  			RTC_DateStructure.RTC_Month = RTC_Month_November;
  			break;
  		case 11:
  			RTC_DateStructure.RTC_Month = RTC_Month_December;
  			break;
  	}
	RTC_DateStructure.RTC_Date = timeinfo->tm_mday; 
	RTC_DateStructure.RTC_Year = timeinfo->tm_year-100;

	switch (timeinfo->tm_wday) {
		case 0:
			RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Sunday;
			break;
		case 1:
			RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Monday;
			break;
		case 2:
			RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Tuesday;
			break;
		case 3:
			RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Wednesday;
			break;
		case 4:
			RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Thursday;
			break;
		case 5:
			RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Friday;
			break;
		case 6:
			RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Saturday;
			break;
		
	}

	/* Calendar Configuration */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	RTC_InitStructure.RTC_SynchPrediv =  0xFF;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);

    /* Set Current Time and Date */
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure); 
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
}

boolean RTCClass::isCalibrated() {
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) == 0x0000) {
		return false;
	} else {
		return true;
	}
}

void RTCClass::getTimestruct(struct tm * timeStruct) {
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

	timeStruct->tm_sec = RTC_TimeStructure.RTC_Seconds;
	timeStruct->tm_min = RTC_TimeStructure.RTC_Minutes;
	timeStruct->tm_hour = RTC_TimeStructure.RTC_Hours;
	timeStruct->tm_mday = RTC_DateStructure.RTC_Date;
	timeStruct->tm_mon = RTC_DateStructure.RTC_Month-1;
	timeStruct->tm_year = RTC_DateStructure.RTC_Year+100;
	timeStruct->tm_wday = RTC_DateStructure.RTC_WeekDay-1;
}

RTCClass Clock;