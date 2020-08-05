#pragma once

#include <Arduino.h>

class InternalClock
{
public:
	enum class Weekday
	{
		Monday = 0, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday
	};
private:
	unsigned long m_second;
	unsigned long m_minute;
	unsigned long m_hour;
	Weekday m_weekday;


public:
	InternalClock(Weekday weekday, byte hour, byte minute, byte second);
	void InternalClockwork();

	void SetTime(Weekday, byte, byte, byte);
	Weekday GetDay();
	byte GetHour();
	byte GetMinute();
	byte GetSecond();
	unsigned long GetTimeInt();
	bool Synclockwork();
};

