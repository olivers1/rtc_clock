#include "InternalClock.h"

InternalClock::InternalClock(Weekday weekday, byte hour, byte minute, byte second)
	: m_weekday(weekday), m_hour(hour), m_minute(minute), m_second(second)
{ 
	
	if ((weekday < Weekday::Monday && weekday > Weekday::Sunday) || (hour < 0 || hour > 23) || (minute < 0 || minute > 59) || (second < 0 && second > 59))
	{
		Serial.println("Time object initialization failed due to invalid input");
		for (;;);	// don't proceed, loop forever
	}
}

void InternalClock::InternalClockwork()
{
	// increase second value each time function is called
	m_second++;

	switch (m_second) {		// catch second counter overflow
	case 60:
		m_minute++;		// increase minute counter
		m_second = 0;	// clear second counter
		break;
	}

	switch (m_minute) {		// catch minute counter overflow
	case 60:
		m_hour++;	// increase hour counter
		m_minute = 0;	// clear minute counter
		break;
	}

	switch (m_hour) {	// catch hour counter overflow
	case 24:
		m_hour = 0;		// clear hour counter
		switch (m_weekday) {	// update weekday when time passes midnight
		case Weekday::Monday:
			m_weekday = Weekday::Tuesday;
			break;
		case Weekday::Tuesday:
			m_weekday = Weekday::Wednesday;
			break;
		case Weekday::Wednesday:
			m_weekday = Weekday::Thursday;
			break;
		case Weekday::Thursday:
			m_weekday = Weekday::Friday;
			break;
		case Weekday::Friday:
			m_weekday = Weekday::Saturday;
			break;
		case Weekday::Saturday:
			m_weekday = Weekday::Sunday;
			break;
		case Weekday::Sunday:
			m_weekday = Weekday::Monday;
			break;
		}
	}
}

unsigned long InternalClock::GetTimeInt()
{
	// convert time to single value data type variable
	unsigned long currentTime = 0;
	currentTime += m_hour * 10000;
	currentTime += m_minute * 100;
	currentTime += m_second * 1;

	if (currentTime < 6000)	// prevent current time value 00'00'00 as being considered as less than 23'00'00
	{
		currentTime += 2400000;
	}

	return currentTime;
}