/*
DS3231.cpp: DS3231 Real-Time Clock library
Eric Ayars
4/1/11

Spliced in DateTime all-at-once reading (to avoid rollover) and unix time
from Jean-Claude Wippler and Limor Fried
Andy Wickert
5/15/11

Fixed problem with SD processors(no function call) by replacing all occurences of the term PM, which
is defined as a macro on SAMD controllers by PM_time.
Simon Gassner
11/28/2017

Fixed setting 12-hour clock in setHour function so that 12:xx AM is not stored as 00:xx and corrected
the setting of the PM flag for 12:xx PM.  These address certain DS3231 errors in properly setting the
AM/PM (bit 5) flag in the 02h register when passing from AM to PM and PM to AM.
David Merrifield
04/14/2020

Changed parameter to uint16_t in isleapYear() because the function performs 16-bit arithmetic
at (y % 400) and because date2days() calls it with a uint16_t parameter. Grouped and typecast certain parameters and intermediate results in the constructor DateTime::DateTime (uint32_t t) to resolve a couple of non-fatal compiler warnings.
David Sparks
08 Sept 2022

Released into the public domain.
*/

#include "DS3231.h"
#include "stdio.h"
#include "string.h"

const uint8_t CLOCK_ADDRESS = 0x68 << 1;
#define SECONDS_FROM_1970_TO_2000 946684800

DS3231 RTC_DS3231;


// Utilities from JeeLabs/Ladyada

////////////////////////////////////////////////////////////////////////////////
// utility code, some of this could be exposed in the DateTime API if needed

// DS3231 is smart enough to know this, but keeping it for now so I don't have
// to rewrite their code. -ADW
static const uint8_t daysInMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

//// number of days since 2000/01/01, valid for 2001..2099
//static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
//    if (y >= 2000)
//        y -= 2000;
//    uint16_t days = d;
//    for (uint8_t i = 1; i < m; ++i)
//        days += (daysInMonth + i - 1);
//    if (m > 2 && isleapYear(y))
//        ++days;
//    return days + 365 * y + (y + 3) / 4 - 1;
//}

//static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
//    return ((days * 24L + h) * 60 + m) * 60 + s;
//}

/*****************************************
	Public Functions
 *****************************************/

/*******************************************************************************
 * TO GET ALL DATE/TIME INFORMATION AT ONCE AND AVOID THE CHANCE OF ROLLOVER
 * DateTime implementation spliced in here from Jean-Claude Wippler's (JeeLabs)
 * RTClib, as modified by Limor Fried (Ladyada); source code at:
 * https://github.com/adafruit/RTClib
 ******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
// DateTime implementation - ignores time zones and DST changes
// NOTE: also ignores leap seconds, see http://en.wikipedia.org/wiki/Leap_second

//DateTime::DateTime (uint32_t t) {
//  t -= SECONDS_FROM_1970_TO_2000;    // bring to 2000 timestamp from 1970

//    ss = t % 60;
//    t /= 60;
//    mm = t % 60;
//    t /= 60;
//    hh = t % 24;
//    uint16_t days = t / 24;
//    uint8_t leap;
//    for (yOff = 0; ; ++yOff) {
//        leap = isleapYear((uint16_t) yOff);
//        if (days < (uint16_t)(365 + leap))
//            break;
//        days -= (365 + leap);
//    }
//    for (m = 1; ; ++m) {
//        uint8_t daysPerMonth = (daysInMonth + m - 1);
//        if (leap && m == 2)
//            ++daysPerMonth;
//        if (days < daysPerMonth)
//            break;
//        days -= daysPerMonth;
//    }
//    d = days + 1;
//}

DateTime::DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
    if (year >= 2000)
        year -= 2000;
    yOff = year;
    m = month;
    d = day;
    hh = hour;
    mm = min;
    ss = sec;
}

// supported formats are date "Mmm dd yyyy" and time "hh:mm:ss" (same as __DATE__ and __TIME__)
DateTime::DateTime(const char* date, const char* time) {
   static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
   static char buff[4] = {'0','0','0','0'};
   int y;
   sscanf(date, "%s %hhu %d", buff, &d, &y);
   yOff = y >= 2000 ? y - 2000 : y;
   m = (strstr(month_names, buff) - month_names) / 3 + 1;
   sscanf(time, "%hhu:%hhu:%hhu", &hh, &mm, &ss);
}

// UNIX time: IS CORRECT ONLY WHEN SET TO UTC!!!
uint32_t DateTime::unixtime(void) const {
//  uint32_t t;
//  uint16_t days = date2days(yOff, m, d);
//  t = time2long(days, hh, mm, ss);
//  t += SECONDS_FROM_1970_TO_2000;  // seconds from 1970 to 2000

//  return t;
}

// Slightly modified from JeeLabs / Ladyada
// Get all date/time at once to avoid rollover (e.g., minute/second don't match)
static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
// Commented to avoid compiler warnings, but keeping in case we want this
// eventually
//static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }

// Sept 2022 changed parameter to uint16_t from uint8_t
bool isleapYear(const uint16_t y) {
  if(y&3)//check if divisible by 4
    return false;
  //only check other, when first failed
  return (y % 100 || y % 400 == 0);
}

//DateTime RTClib::now(TwoWire & _Wire) {
//  _Wire.beginTransmission(CLOCK_ADDRESS);
//  _Wire.write(0);	// This is the first register address (Seconds)
//  			// We'll read from here on for 7 bytes: secs reg, minutes reg, hours, days, months and years.
//  _Wire.endTransmission();

//  _Wire.requestFrom(CLOCK_ADDRESS, 7);
//  uint16_t ss = bcd2bin(_Wire.read() & 0x7F);
//  uint16_t mm = bcd2bin(_Wire.read());
//  uint16_t hh = bcd2bin(_Wire.read());
//  _Wire.read();
//  uint16_t d = bcd2bin(_Wire.read());
//  uint16_t m = bcd2bin(_Wire.read());
//  uint16_t y = bcd2bin(_Wire.read()) + 2000;

//  return DateTime (y, m, d, hh, mm, ss);
//}


/*! -------------------------------------------------------------------------
\brief Get seconds from DS3231
\details  
*/
bool DS3231::getSeconds(uint8_t &seconds) {
	bool function_failed = true;
	
	uint8_t temp_buffer;
	if(!Phy.ReadI2C1_DS3231(CLOCK_ADDRESS, DS3231_SECONDS, &temp_buffer, 1)) {
		seconds =  bcdToDec(temp_buffer);
		function_failed = false;
	}
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Get minutes from DS3231
\details  
*/
bool DS3231::getMinutes(uint8_t &minutes) {
	bool function_failed = true;
	
	uint8_t temp_buffer;
	if(!Phy.ReadI2C1_DS3231(CLOCK_ADDRESS, DS3231_MINUTES, &temp_buffer, 1)) {
		minutes =  bcdToDec(temp_buffer);
		function_failed = false;
	}
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Get hours from DS3231
\details  
h12 = 0 - PM time format
h12 = 1 - AM time format

*/
bool DS3231::getHours(uint8_t &hours, bool &h12, bool& PM_time) {
	bool function_failed = true;
	uint8_t temp_buffer;
	
	if(!Phy.ReadI2C1_DS3231(CLOCK_ADDRESS, DS3231_HOURS, &temp_buffer, 1)) {
		h12 = temp_buffer & DS3231_HOURS_12_24_MSK;
		if (h12) {
			PM_time = temp_buffer & DS3231_HOURS_AM_PM_MSK;
			hours = bcdToDec(temp_buffer & DS3231_HOURS_PM_TIME);
		} 
		else {
			hours = bcdToDec(temp_buffer & DS3231_HOURS_AM_TIME);
		}
		function_failed = false;
	}
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Get day of week from DS3231
\details  
*/
bool DS3231::getDoW(uint8_t &day_of_week) {
	bool function_failed = true;
	
	uint8_t temp_buffer;
	if(!Phy.ReadI2C1_DS3231(CLOCK_ADDRESS, DS3231_DAY, &temp_buffer, 1)) {
		day_of_week =  bcdToDec(temp_buffer);
		function_failed = false;
	}
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Get date from DS3231
\details  
*/
bool DS3231::getDate(uint8_t &date) {
	bool function_failed = true;
	
	uint8_t temp_buffer;
	if(!Phy.ReadI2C1_DS3231(CLOCK_ADDRESS, DS3231_DATE, &temp_buffer, 1)) {
		date =  bcdToDec(temp_buffer);
		function_failed = false;
	}
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Get month from DS3231
\details  
*/
bool DS3231::getMonth(uint8_t &month, bool& century) {
	bool function_failed = true;
	
	uint8_t temp_buffer;
	if(!Phy.ReadI2C1_DS3231(CLOCK_ADDRESS, DS3231_MONTH_CENTURY, &temp_buffer, 1)) {
		century = temp_buffer & DS3231_CENTURY_MSK;
		month =  bcdToDec(temp_buffer & DS3231_MONTH_MSK);
		function_failed = false;
	}
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Get year from DS3231
\details  
*/
bool DS3231::getYear(uint8_t &year) {
	bool function_failed = true;
	
	uint8_t temp_buffer;
	if(!Phy.ReadI2C1_DS3231(CLOCK_ADDRESS, DS3231_YEAR, &temp_buffer, 1)) {
		year =  bcdToDec(temp_buffer);
		function_failed = false;
	}
	return function_failed;
}

// setEpoch function gives the epoch as parameter and feeds the RTC
// epoch = UnixTime and starts at 01.01.1970 00:00:00
// HINT: => the AVR time.h Lib is based on the year 2000
void DS3231::setEpoch(time_t epoch, bool flag_localtime) {
//#if defined (__AVR__)
//	epoch -= SECONDS_FROM_1970_TO_2000;
//#endif
//	struct tm tmnow;
//	if (flag_localtime) {
//		localtime_r(&epoch, &tmnow);
//	}
//	else {
//		gmtime_r(&epoch, &tmnow);
//	}
//	setSecond(tmnow.tm_sec);
//	setMinute(tmnow.tm_min);
//	setHour(tmnow.tm_hour);
//	setDoW(tmnow.tm_wday + 1U);
//	setDate(tmnow.tm_mday);
//	setMonth(tmnow.tm_mon + 1U);
//	setYear(tmnow.tm_year - 100U);
}


/*! -------------------------------------------------------------------------
\brief Set seconds to DS3231
\details  This function also resets the Oscillator Stop Flag, which is set
whenever power is interrupted.
*/
bool DS3231::setSecond(const uint8_t seconds) {
	bool function_failed = true;
	
	uint8_t bcd_seconds = decToBcd(seconds);
	if(!Phy.WriteI2C1_DS3231(CLOCK_ADDRESS, DS3231_SECONDS, &bcd_seconds, 1)) {
		// Clear OSF flag
		uint8_t temp_buffer = 0;
		function_failed = readControlByte(temp_buffer, 1);
		function_failed |= writeControlByte((temp_buffer & 0x7F), 1);
	}
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Set minutes to DS3231
\details  
*/
bool DS3231::setMinute(const uint8_t minutes) {
	uint8_t bcd_minutes = decToBcd(minutes);
	bool function_failed = Phy.WriteI2C1_DS3231(CLOCK_ADDRESS, DS3231_MINUTES, &bcd_minutes, 1);
	return function_failed;
}

// Following setHour revision by David Merrifield 4/14/2020 correcting handling of 12-hour clock

/*! -------------------------------------------------------------------------
\brief Set hours to DS3231
\details  Sets the hour, without changing 12/24h mode. The hour must be in 24h format.
*/
bool DS3231::setHour(const uint8_t hour) {
	bool function_failed = true;
	bool h12;
	uint8_t temp_hour;
	
	if(!Phy.ReadI2C1_DS3231(CLOCK_ADDRESS, DS3231_HOURS, &temp_hour, 1)) {
		h12 = (temp_hour & DS3231_HOURS_12_24_MSK);
		
		if (h12) {																																// 12 hour
			bool am_pm = (hour > 11);
			temp_hour = hour;
			if (temp_hour > 11) {
				temp_hour = temp_hour - 12;
			}
			if (temp_hour == 0) {
				temp_hour = 12;
			}
			temp_hour = decToBcd(temp_hour) | (am_pm << 5) | DS3231_HOURS_12_24_MSK;
		} 
		else {																																		// 24 hour
			temp_hour = decToBcd(hour) & 0xBF;
		}
		
		function_failed = Phy.WriteI2C1_DS3231(CLOCK_ADDRESS, DS3231_HOURS, &temp_hour, 1);
	
	}

	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Set days of week to DS3231
\details  
*/
bool DS3231::setDoW(const uint8_t DoW) {
	uint8_t BcdDow = decToBcd(DoW);
	bool function_failed = Phy.WriteI2C1_DS3231(CLOCK_ADDRESS, DS3231_DAY, &BcdDow, 1);
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Set date to DS3231
\details  
*/
bool DS3231::setDate(const uint8_t date) {
	uint8_t Bcd_date = decToBcd(date);
	bool function_failed = Phy.WriteI2C1_DS3231(CLOCK_ADDRESS, DS3231_DATE, &Bcd_date, 1);
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Set month to DS3231
\details  
*/
bool DS3231::setMonth(const uint8_t month) {
	uint8_t bcd_month = decToBcd(month);
	bool function_failed = Phy.WriteI2C1_DS3231(CLOCK_ADDRESS, DS3231_MONTH_CENTURY, &bcd_month, 1);
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Set year to DS3231
\details  
*/
bool DS3231::setYear(const uint8_t year) {
	uint8_t bcd_year = decToBcd(year);
	bool function_failed = Phy.WriteI2C1_DS3231(CLOCK_ADDRESS, DS3231_YEAR, &bcd_year, 1);
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Set clock mode to DS3231
\details sets the mode to 12-hour (true) or 24-hour (false).
	 One thing that bothers me about how I've written this is that
	 if the read and right happen at the right hourly millisecnd,
	 the clock will be set back an hour. Not sure how to do it better,
	 though, and as long as one doesn't set the mode frequently it's
	 a very minimal risk.
	 It's zero risk if you call this BEFORE setting the hour, since
	 the setHour() function doesn't change this mode.
*/
bool DS3231::setClockMode(bool h12) {
	bool function_failed;
	uint8_t temp_buffer;
	
	if(!Phy.ReadI2C1_DS3231(CLOCK_ADDRESS, DS3231_HOURS, &temp_buffer, 1)) {
		h12 = (temp_buffer & DS3231_HOURS_12_24_MSK);
		
		// Set the flag to the requested value:
		if (h12) {
			temp_buffer = temp_buffer | DS3231_HOURS_12_24_MSK;
		} else {
			temp_buffer = temp_buffer & 0xBF;
		}
		function_failed = Phy.WriteI2C1_DS3231(CLOCK_ADDRESS, DS3231_HOURS, &temp_buffer, 1);
	}
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Get temperature from DS3231
\details  Checks the internal thermometer on the DS3231 and returns the
temperature as a floating-point value.
*/
bool DS3231::getTemperature(float &temperature) {
	bool function_failed = true;
	uint8_t data[2];
  float temp3231;

  // temp registers (11h-12h) get updated automatically every 64s
	if(!Phy.ReadI2C1_DS3231(CLOCK_ADDRESS, DS3231_TEMPERATURE_MSB, data, 2)) {
    int16_t  itemp  = ( data[0] << 8 | (data[1] & 0xC0) );  									// Shift upper byte, add lower
    temperature = ( (float)itemp / 256.0 );              											// Scale and return
		function_failed = false;
  }

  return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Get A1 alarms from DS3231
\details  
*/
bool DS3231::getA1Time(uint8_t& A1Day, uint8_t& A1Hour, uint8_t& A1Minute, uint8_t& A1Second, uint8_t& AlarmBits, bool& A1Dy, bool& A1h12, bool& A1PM) {
	bool function_failed = true;
	
	uint8_t temp_buffer[7];
	if(!Phy.ReadI2C1_DS3231(CLOCK_ADDRESS, DS3231_ALARM_1_SECONDS, temp_buffer, 1)) {
		
		// Get A1M1 and A1 Seconds
		A1Second	= bcdToDec(temp_buffer[0] & DS3231_SECONS_REG_MSK);
		AlarmBits	= AlarmBits | (temp_buffer[0] & DS3231_ALARM_BIT_MSK) >> 7;				 	// put A1M1 bit in position 0 of DS3231_AlarmBits.

		// Get A1M2 and A1 Minutes
		A1Minute	= bcdToDec(temp_buffer[1] & DS3231_MINUTES_REG_MSK);
		AlarmBits	= AlarmBits | (temp_buffer[1] & DS3231_ALARM_BIT_MSK) >> 6;					// put A1M2 bit in position 1 of DS3231_AlarmBits.

		// Get A1M3 and A1 Hour
		AlarmBits	= AlarmBits | (temp_buffer[2] & DS3231_ALARM_BIT_MSK) >> 5;					// put A1M3 bit in position 2 of DS3231_AlarmBits.
		A1h12		= temp_buffer[2] & DS3231_HOURS_12_24_MSK;														// determine A1 12/24 mode
		if (A1h12) {
			A1PM	= temp_buffer[2] & DS3231_HOURS_AM_PM_MSK;														// determine am/pm
			A1Hour	= bcdToDec(temp_buffer[2] & DS3231_HOURS_PM_TIME);									// 12-hour
		} 
		else {
			A1Hour	= bcdToDec(temp_buffer[2] & DS3231_HOURS_AM_TIME);									// 24-hour
		}

		// Get A1M4 and A1 Day/Date
		AlarmBits	= AlarmBits | (temp_buffer[3] & DS3231_ALARM_BIT_MSK) >> 4;					// put A1M3 bit in position 3 of DS3231_AlarmBits.
		A1Dy		= (temp_buffer[3] & DS3231_DAY_REG_MSK) >> 6;													// determine A1 day or date flag
		if (A1Dy) {	
			A1Day	= bcdToDec(temp_buffer[3] & DS3231_DAY_MSK);													// alarm is by day of week, not date.
		} 
		else {
			A1Day	= bcdToDec(temp_buffer[3] & DS3231_DATE_MSK);													// alarm is by date, not day of week.
		}
		
		function_failed = false;
	}
	return function_failed;
}

void DS3231::getA1Time(uint8_t& A1Day, uint8_t& A1Hour, uint8_t& A1Minute, uint8_t& A1Second, uint8_t& AlarmBits, bool& A1Dy, bool& A1h12, bool& A1PM, bool clearAlarmBits) {
    if (clearAlarmBits) {
        AlarmBits = 0x0;
    }
    getA1Time(A1Day, A1Hour, A1Minute, A1Second, AlarmBits, A1Dy, A1h12, A1PM);
}

/*! -------------------------------------------------------------------------
\brief Get A2 alarms from DS3231
\details  
*/
bool DS3231::getA2Time(uint8_t& A2Day, uint8_t& A2Hour, uint8_t& A2Minute, uint8_t& AlarmBits, bool& A2Dy, bool& A2h12, bool& A2PM) {
		bool function_failed = true;
	
	uint8_t temp_buffer[3];
	if(!Phy.ReadI2C1_DS3231(CLOCK_ADDRESS, DS3231_ALARM_2_MINUTES, temp_buffer, 1)) {
		
		// Get A2M2 and A1 Minutes
		A2Minute	= bcdToDec(temp_buffer[0] & DS3231_MINUTES_REG_MSK);
		AlarmBits	= AlarmBits | (temp_buffer[0] & DS3231_ALARM_BIT_MSK) >> 6;					// put A2M2 bit in position 1 of DS3231_AlarmBits.

		// Get A1M3 and A1 Hour
		AlarmBits	= AlarmBits | (temp_buffer[1] & DS3231_ALARM_BIT_MSK) >> 5;					// put A1M3 bit in position 2 of DS3231_AlarmBits.
		A2h12		= temp_buffer[2] & DS3231_HOURS_12_24_MSK;														// determine A1 12/24 mode
		if (A2h12) {
			A2PM	= temp_buffer[2] & DS3231_HOURS_AM_PM_MSK;														// determine am/pm
			A2Hour	= bcdToDec(temp_buffer[1] & DS3231_HOURS_PM_TIME);									// 12-hour
		} 
		else {
			A2Hour	= bcdToDec(temp_buffer[1] & DS3231_HOURS_AM_TIME);									// 24-hour
		}

		// Get A1M4 and A1 Day/Date
		AlarmBits	= AlarmBits | (temp_buffer[2] & DS3231_ALARM_BIT_MSK) >> 4;					// put A1M3 bit in position 3 of DS3231_AlarmBits.
		A2Dy		= (temp_buffer[2] & DS3231_DAY_REG_MSK) >> 6;													// determine A1 day or date flag
		if (A2Dy) {	
			A2Day	= bcdToDec(temp_buffer[2] & DS3231_DAY_MSK);													// alarm is by day of week, not date.
		} 
		else {
			A2Day	= bcdToDec(temp_buffer[2] & DS3231_DATE_MSK);													// alarm is by date, not day of week.
		}
		
		function_failed = false;
	}
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Get A2 alarms from DS3231
\details  
*/
void DS3231::getA2Time(uint8_t& A2Day, uint8_t& A2Hour, uint8_t& A2Minute, uint8_t& AlarmBits, bool& A2Dy, bool& A2h12, bool& A2PM, bool clearAlarmBits) {
    if (clearAlarmBits) {
        AlarmBits = 0x0;
    }
    getA2Time(A2Day, A2Hour, A2Minute, AlarmBits, A2Dy, A2h12, A2PM);
}

/*! -------------------------------------------------------------------------
\brief 	Sets the alarm-1 date and time on the DS3231, using A1* information
\details  
*/
bool DS3231::setA1Time(uint8_t A1Day, uint8_t A1Hour, uint8_t A1Minute, uint8_t A1Second, uint8_t AlarmBits, bool A1Dy, bool A1h12, bool A1PM) {	
	uint8_t temp_buffer[4] = {0, 0, 0, 0};
	temp_buffer[0] = (decToBcd(A1Second) | ((AlarmBits & 0x01) << 7));
	temp_buffer[1] = (decToBcd(A1Minute) | ((AlarmBits & 0x02) << 6));
	
		// Figure out A1 hour
	if (A1h12) {																						// Start by converting existing time to h12 if it was given in 24h.
		if (A1Hour > 12) {
			A1Hour = A1Hour - 12;																// well, then, this obviously isn't a h12 time, is it?
			A1PM = true;
		}
		if (A1PM) {
			
			temp_buffer[2] = decToBcd(A1Hour) | 0x60;						// Afternoon. Convert the hour to BCD and add appropriate flags.
		} 
		else {
			temp_buffer[2] = decToBcd(A1Hour) | 0x40;						// Morning. Convert the hour to BCD and add appropriate flags.
		}
	} 
	else {
		temp_buffer[2] = decToBcd(A1Hour);										// Now for 24h
	}
	temp_buffer[2] = temp_buffer[2] | ((AlarmBits & 0x04) << 5);

	// Figure out A1 day/date and A1M4
	temp_buffer[3] = ((AlarmBits & 0x08) << 4) | decToBcd(A1Day);
	if (A1Dy) {
		temp_buffer[3] = temp_buffer[3] | 0x40;							// Set A1 Day/Date flag (Otherwise it's zero)
	}
	
	return (Phy.WriteI2C1_DS3231(CLOCK_ADDRESS, DS3231_ALARM_1_SECONDS, temp_buffer, 4));
}

/*! -------------------------------------------------------------------------
\brief 	Sets the alarm-2 date and time on the DS3231, using A2* information
\details  
*/
bool DS3231::setA2Time(uint8_t A2Day, uint8_t A2Hour, uint8_t A2Minute, uint8_t AlarmBits, bool A2Dy, bool A2h12, bool A2PM) {
	uint8_t temp_buffer[3] = {0, 0, 0};
	temp_buffer[0] = (decToBcd(A2Minute) | ((AlarmBits & 0x02) << 6));
	
		// Figure out A2 hour
	if (A2h12) {																						// Start by converting existing time to h12 if it was given in 24h.
		if (A2Hour > 12) {
			A2Hour = A2Hour - 12;																// well, then, this obviously isn't a h12 time, is it?
			A2PM = true;
		}
		if (A2PM) {
			temp_buffer[1] = decToBcd(A2Hour) | 0x60;						// Afternoon. Convert the hour to BCD and add appropriate flags.
		} 
		else {
			temp_buffer[1] = decToBcd(A2Hour) | 0x40;						// Morning. Convert the hour to BCD and add appropriate flags.
		}
	} 
	else {
		temp_buffer[1] = decToBcd(A2Hour);										// Now for 24h
	}
	temp_buffer[1] = temp_buffer[1] | ((AlarmBits & 0x04) << 5);

	// Figure out A1 day/date and A1M4
	temp_buffer[2] = ((AlarmBits & 0x08) << 4) | decToBcd(A2Day);
	if (A2Dy) {
		temp_buffer[2] = temp_buffer[2] | 0x40;							// Set A1 Day/Date flag (Otherwise it's zero)
	}
	
	return (Phy.WriteI2C1_DS3231(CLOCK_ADDRESS, DS3231_ALARM_1_SECONDS, temp_buffer, 3));
}

/*! -------------------------------------------------------------------------
\brief Turns on alarm number "Alarm". Defaults to 2 if Alarm is not 1
\details  Leaves interrupt pin alone.
*/
bool DS3231::turnOnAlarm(const uint8_t alarm_number) {
	uint8_t temp_buffer = 0;
	bool function_failed = readControlByte(temp_buffer, 0);
	
	// modify control byte
	if (alarm_number == 1) {
		temp_buffer |= A1IE_MSK;
	} 
	else {
		temp_buffer |= A2IE_MSK;
	}
	function_failed |= writeControlByte(temp_buffer, 0);
	
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Turns off alarm number "Alarm". Defaults to 2 if Alarm is not 1.
\details  Leaves interrupt pin alone.
*/
bool DS3231::turnOffAlarm(const uint8_t alarm_number) {
	uint8_t temp_buffer = 0;
	bool function_failed = readControlByte(temp_buffer, 0);
	
	// modify control byte
	if (alarm_number == 1) {
		temp_buffer &= ~A1IE_MSK;
	} 
	else {
		temp_buffer &= ~A2IE_MSK;
	}
	function_failed |= writeControlByte(temp_buffer, 0);
	
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Checks whether the given alarm is enabled
\details  
*/
bool DS3231::checkAlarmEnabled(bool &alarm_en, uint8_t alarm_number) {
	bool function_failed = true;
	uint8_t temp_buffer = 0;
	
	if(!readControlByte(temp_buffer, 0)) {
		if(alarm_number == 1) {
			alarm_en = temp_buffer & A1IE_MSK;
		} 
		else {
			alarm_en = temp_buffer & A2IE_MSK;
		}
		function_failed = false;
	}
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Checks whether alarm 1 or alarm 2 flag is on, returns T/F accordingly.
\details  Turns flag off, also. Defaults to checking alarm 2, unless Alarm == 1.
*/
bool DS3231::checkIfAlarm(bool &alarm_happen, uint8_t alarm_number) {
	bool function_failed = true;
	uint8_t result;
	uint8_t temp_buffer = 0;
	
	if(!readControlByte(temp_buffer, 1)) {
		if (alarm_number == 1) {
			result = temp_buffer & A1F_MSK;					// Did alarm 1 go off?
			temp_buffer &= ~A1F_MSK;								// clear flag
		} 
		else {
			result = temp_buffer & A2F_MSK;					// Did alarm 2 go off?
			temp_buffer &= ~A2F_MSK;								// clear flag
		}	

		function_failed = writeControlByte(temp_buffer, 1);
	}
	return result;
}

/*! -------------------------------------------------------------------------
\brief Checks whether alarm 1 or alarm 2 flag is on, returns T/F accordingly.
\details  Clears flag, if clearflag is set defaults to checking alarm 2, unless Alarm == 1.
*/
bool DS3231::checkIfAlarm(bool &alarm_happen, uint8_t alarm_number, bool clearflag) {
	bool function_failed = true;
	uint8_t result;
	uint8_t temp_buffer = 0;
	
	if(!readControlByte(temp_buffer, 1)) {
		if (alarm_number == 1) {
			result = temp_buffer & A1F_MSK;					// Did alarm 1 go off?
			temp_buffer &= ~A1F_MSK;								// clear flag
		} 
		else {
			result = temp_buffer & A2F_MSK;					// Did alarm 2 go off?
			temp_buffer &= ~A2F_MSK;								// clear flag
		}	
		function_failed = false;
		
		if (clearflag) {
			function_failed |= writeControlByte(temp_buffer, 1);
		}
	}
	return result;
}

/*! -------------------------------------------------------------------------
\brief Enable oscillator of DS3231
\details  
	turns oscillator on or off. True is on, false is off.
	if battery is true, turns on even for battery-only operation,
	otherwise turns off if Vcc is off.
	frequency must be 0, 1, 2, or 3.
	0 = 1 Hz
	1 = 1.024 kHz
	2 = 4.096 kHz
	3 = 8.192 kHz (Default if frequency byte is out of range)
*/
bool DS3231::enableOscillator(bool TF, bool battery, uint8_t frequency) {
	bool function_failed = true;
	
	if (frequency > 3) frequency = 3;
	
	uint8_t temp_buffer = 0;
	if(!readControlByte(temp_buffer, 0)) {
		temp_buffer &= 0xE7;																		// read control byte in, but zero out current state of RS2 and RS1.
		if (battery) {
			temp_buffer |= BBSQW_MSK;															// turn on BBSQW flag
		} 
		else {
			temp_buffer &= ~BBSQW_MSK;														// turn off BBSQW flag
		}
		if (TF) {
			temp_buffer &= ~nEOSC_MSK;														// set ~EOSC to 0 and INTCN to zero.
		} 
		else {
			temp_buffer |= nEOSC_MSK;															// set ~EOSC to 1, leave INTCN as is.
		}
		
		frequency <<= 3;																				// shift frequency into bits 3 and 4 and set.
		temp_buffer |= frequency;
		
		function_failed =  writeControlByte(temp_buffer, 0);		// And write the control bits
	}
	
	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Check oscillator of DS3231
\details  Turn 32kHz pin on or off
*/
bool DS3231::enable32kHz(bool TF) {
	bool function_failed = true;
	
	uint8_t temp_buffer = 0; 
	if(!readControlByte(temp_buffer, 1)) {
		if (TF) {
			temp_buffer = temp_buffer | EN32_KHZ_MSK;				// turn on 32kHz pin
		} 
		else {																			
			temp_buffer = temp_buffer & ~EN32_KHZ_MSK;			// turn off 32kHz pin
		}
		function_failed = writeControlByte(temp_buffer, 1);
	}

	return function_failed;
}

/*! -------------------------------------------------------------------------
\brief Check oscillator of DS3231
\details  Returns false if the oscillator has been off for some reason.
If this is the case, the time is probably not correct.
*/
bool DS3231::oscillatorCheck(bool &stop) {
	bool function_failed = true;
	uint8_t temp_buffer = 0; 
	if(!readControlByte(temp_buffer, 1)) {
		stop = false;
		if (temp_buffer & DS3231_OSCIL_BIT_MSK) {			// Oscillator Stop Flag (OSF) is set, so return false.
			stop = true;
		}
		function_failed = false;
	}

	return function_failed;
}

/*****************************************
	Private Functions
 *****************************************/

/*! -------------------------------------------------------------------------
\brief Convert normal decimal numbers to binary coded decimal
\details 
*/
uint8_t DS3231::decToBcd(uint8_t val) {
	return ( (val/10*16) + (val%10) );
}

/*! -------------------------------------------------------------------------
\brief Convert binary coded decimal to normal decimal numbers
\details 
*/
uint8_t DS3231::bcdToDec(uint8_t val) {
	return ( (val/16*10) + (val%16) );
}

/*! -------------------------------------------------------------------------
\brief Read Control Byte
\details  Read selected control byte, first byte (0) is 0x0e, second (1) is 0x0f
*/
bool DS3231::readControlByte(uint8_t &control_byte, bool which) {
	bool function_faled = false;
	if (which) {
		function_faled = Phy.ReadI2C1_DS3231(CLOCK_ADDRESS, DS3231_CONTROL_STATUS, &control_byte, 1);		// second control byte
	} 
	else {
		function_faled = Phy.ReadI2C1_DS3231(CLOCK_ADDRESS, DS3231_CONTROL, &control_byte, 1);					// first control byte
	}
	return function_faled;
}

/*! -------------------------------------------------------------------------
\brief Write Control Byte
\details  Write the selected control byte. Which=false -> 0x0e, true->0x0f.
*/
bool DS3231::writeControlByte(uint8_t control_byte, bool which) {
	bool function_failed = true;

	if (which) {
		function_failed = Phy.WriteI2C1_DS3231(CLOCK_ADDRESS, DS3231_CONTROL_STATUS, &control_byte, 1);		// second control byte
	} else {
		function_failed = Phy.WriteI2C1_DS3231(CLOCK_ADDRESS, DS3231_CONTROL, &control_byte, 1);					// first control byte
	}
	return function_failed;
}
