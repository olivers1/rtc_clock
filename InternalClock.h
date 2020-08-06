#pragma once

#include <Arduino.h>

class InternalClock
{
public:
	enum class Weekday
	{
		Monday = 0, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday
	};
	enum class TimeZone
	{
		WinterTime=1, SummerTime
	};

private:
	unsigned long m_hour;
	unsigned long m_minute;
	unsigned long m_second;
	Weekday m_weekday;
	TimeZone m_timeZone;
public:
	InternalClock();
	InternalClock(Weekday weekday, byte hour, byte minute, byte second);
	void SetWeekdayTimeZone(Weekday, TimeZone);
	void InternalClockwork();
	void SyncClockworkNTP(unsigned long);

	Weekday GetDay();
	byte GetHour();
	byte GetMinute();
	byte GetSecond();
	unsigned long GetTimeInt();
};

