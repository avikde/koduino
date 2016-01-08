#ifndef RTC_H
#define RTC_H

#include "Arduino.h"
#include <time.h>

class RTCClass {
private:
	// boolean rtcCalibrated;

public:
	RTCClass(void);

	void setTime(time_t localTime);
	boolean isCalibrated();
	void getTimestruct(struct tm * timeStruct);

};

extern RTCClass Clock;

#endif